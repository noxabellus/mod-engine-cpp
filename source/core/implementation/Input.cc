#include "../include/Input.hh"



namespace mod {
  void KeyboardInput::process_sdl_input (SDL_Event const& event) {
    using namespace Keycode;

    switch (event.type) {
      case SDL_KEYDOWN: {
        u8_t keycode = from_sdl(event.key.keysym.sym);
        
        if (validate(keycode)) {
          if (ModifierKey::validate_key(keycode)) active_modifiers.set(ModifierKey::from_keycode(keycode));
          else if (!LockKey::validate_key(keycode)) active_keys.set(keycode);
        }
      } break;

      case SDL_KEYUP: {
        u8_t keycode = from_sdl(event.key.keysym.sym);

        if (validate(keycode)) {
          if (LockKey::validate_key(keycode)) active_locks.toggle(LockKey::from_keycode(keycode));
          else if (ModifierKey::validate_key(keycode)) active_modifiers.unset(ModifierKey::from_keycode(keycode));
          else active_keys.unset(keycode);
        }
      } break;

      default: break;
    }
  }

  bool KeyboardInput::operator [] (u8_t code) const {
    return active_keys.match_index(code);
  }

  bool KeyboardInput::operator [] (char const* name) const {
    return active_keys.match_index(Keycode::from_name(name));
  }

  bool KeyboardInput::char_key (char ch) const {
    return active_keys.match_index(Keycode::from_char(ch));
  }

  bool KeyboardInput::lock (u8_t lock) const {
    return active_locks.match_index(lock);
  }

  bool KeyboardInput::lock (char const* name) const {
    return active_locks.match_index(LockKey::from_name(name));
  }

  bool KeyboardInput::modifier (u8_t modifier) const {
    return active_modifiers.match_index(modifier);
  }

  bool KeyboardInput::modifier (char const* name) const {
    return active_modifiers.match_index(ModifierKey::from_name(name));
  }

  void KeyboardInput::clear () {
    active_keys.clear();
    active_locks.clear();
    active_modifiers.clear();
  }



  void MouseInput::begin_frame () {
    using namespace MouseButton;

    active_buttons.unset_multiple(WheelUp, WheelDown, WheelLeft, WheelRight);
  }

  void MouseInput::process_sdl_input (SDL_Event const& event) {
    using namespace MouseButton;

    switch (event.type) {
      case SDL_MOUSEBUTTONDOWN: {
        u8_t button = from_sdl(event.button.button);

        if (validate(button)) active_buttons.set(button);
      } break;

      case SDL_MOUSEBUTTONUP: {
        u8_t button = from_sdl(event.button.button);

        if (validate(button)) active_buttons.unset(button);
      } break;

      case SDL_MOUSEMOTION: {
        position.x = event.motion.x;
        position.y = event.motion.y;
      } break;

      case SDL_MOUSEWHEEL: {
        if (event.wheel.x < 0) active_buttons.set(WheelLeft);
        else if (event.wheel.x > 0) active_buttons.set(WheelRight);

        if (event.wheel.y < 0) active_buttons.set(WheelDown);
        else if (event.wheel.y > 0) active_buttons.set(WheelUp);
      }
    }
  }

  bool MouseInput::operator [] (u8_t button) const {
    return active_buttons.match_index(button);
  }

  bool MouseInput::operator [] (char const* name) const {
    return active_buttons.match_index(MouseButton::from_name(name));
  }

  void MouseInput::clear () {
    active_buttons.clear();
    position = { 0, 0 };
  }


  void RawInput::begin_frame () {
    mouse.begin_frame();
  }

  void RawInput::process_sdl_input (SDL_Event const& event) {
    keyboard.process_sdl_input(event);
    mouse.process_sdl_input(event);
  }

  void RawInput::clear () {
    keyboard.clear();
    mouse.clear();
  }


  InputCombination InputCombination::from_json_item (JSONItem const& item) {
    JSONItem* modifiers_item = item.get_object_item("modifiers");
    JSONItem* keys_item = item.get_object_item("keys");
    JSONItem* buttons_item = item.get_object_item("buttons");

    InputCombination input_combo;

    if (modifiers_item != NULL) {
      for (auto [ i, modifier_item ] : modifiers_item->get_array()) {
        String& modifier_s = modifier_item.get_string();
        u8_t modifier = ModifierKey::from_name(modifier_s.value);

        modifier_item.asset_assert(
          ModifierKey::validate(modifier),
          "%s is not a valid ModifierKey name. Valid options are:\n%s",
          modifier_s.value, ModifierKey::valid_values
        );
        
        input_combo.modifiers.set(modifier);
      }
    }

    if (keys_item != NULL) {
      for (auto [ i, key_item ] : keys_item->get_array()) {
        String& key_s = key_item.get_string();
        u8_t key = Keycode::from_name(key_s.value);

        key_item.asset_assert(
          Keycode::validate(key),
          "%s is not a valid Keycode name. Valid options are:\n%s",
          key_s.value, Keycode::valid_values
        );
        
        input_combo.keys.set(key);
      }
    }

    if (buttons_item != NULL) {
      for (auto [ i, button_item ] : buttons_item->get_array()) {
        String& button_s = button_item.get_string();
        u8_t button = MouseButton::from_name(button_s.value);

        button_item.asset_assert(
          MouseButton::validate(button),
          "%s is not a valid MouseButton name. Valid options are:\n%s",
          button_s.value, MouseButton::valid_values
        );
        
        input_combo.buttons.set(button);
      }
    }

    return input_combo;
  }

  JSONItem InputCombination::to_json_item () const {
    JSONObject object;


    if (modifiers.match_any()) {
      JSONArray modifier_arr;

      for (u8_t modifier = 0; modifier < ModifierKey::total_modifier_count; modifier ++) {
        if (modifiers.match_index(modifier)) {
          modifier_arr.append({ ModifierKey::name(modifier) });
        }
      }

      object.set(modifier_arr, "modifiers");
    }

    if (keys.match_any()) {
      JSONArray key_arr;

      for (u8_t key = 0; key < Keycode::total_key_count; key ++) {
        if (keys.match_index(key)) {
          key_arr.append({ Keycode::name(key) });
        }
      }

      object.set(key_arr, "keys");
    }


    if (buttons.match_any()) {
      JSONArray button_arr;

      for (u8_t button = 0; button < MouseButton::total_button_count; button ++) {
        if (buttons.match_index(button)) {
          button_arr.append({ MouseButton::name(button) });
        }
      }

      object.set(button_arr, "buttons");
    }


    return { object };
  }

  bool InputCombination::test (RawInput const& raw_input) const {
    return raw_input.keyboard.active_modifiers.match_subset(modifiers)
        && raw_input.keyboard.active_keys.match_subset(keys)
        && raw_input.mouse.active_buttons.match_subset(buttons);
  }

  void InputCombination::generate_string (String& out) const {
    bool have_modifier = false;

    for (u8_t i = 0; i < ModifierKey::total_modifier_count; i ++) {
      if (modifiers.match_index(i)) {
        if (have_modifier) out.append(" + ");
        out.append(ModifierKey::names[i]);
        have_modifier = true;
      }
    }


    bool have_key = false;

    for (u8_t i = 0; i < Keycode::total_key_count; i ++) {
      if (keys.match_index(i)) {
        if (have_modifier || have_key) out.append(" + ");
        out.append(Keycode::names[i]);
        have_key = true;
      }
    }


    bool have_button = false;

    for (u8_t i = 0; i < MouseButton::total_button_count; i ++) {
      if (buttons.match_index(i)) {
        if (have_modifier || have_key || have_button) out.append(" + ");
        out.fmt_append("Mouse%s", MouseButton::names[i]);
        have_button = true;
      }
    }
  }

  void InputCombination::clear () {
    modifiers.clear();
    keys.clear();
    buttons.clear();
  }


  
  bool Control::test (RawInput const& raw_input) const {
    return input_combination.test(raw_input);
  }

  void Control::clear () {
    input_combination.clear();
  }

  void Control::destroy () {
    if (name != NULL) {
      free(name);
      name = NULL;
    }
  }


  
  JSONItem ControlBinding::to_json_item () const {
    JSONObject object;
    
    for (auto [ i, control ] : controls) {
      object.set({ control.input_combination.to_json_item() }, control.name);
    }

    return { object };
  }


  s64_t ControlBinding::get_index (u32_t id) const {
    if (id != 0) {
      for (auto [ i, control ] : controls) {
        if (id == control.id) return i;
      }
    }

    return -1;
  }


  s64_t ControlBinding::get_index (char const* name) const {
    for (auto [ i, control ] : controls) {
      if (str_cmp_caseless(name, control.name) == 0) return i;
    }

    return -1;
  }


  Control* ControlBinding::get_pointer (u32_t id) const {
    if (id != 0) {
      for (auto [ i, control ] : controls) {
        if (id == control.id) return &control;
      }
    }

    return NULL;
  }


  Control* ControlBinding::get_pointer (char const* name) const {
    for (auto [ i, control ] : controls) {
      if (str_cmp_caseless(name, control.name) == 0) return &control;
    }

    return NULL;
  }


  Control& ControlBinding::get (u32_t id) const {
    Control* ptr = get_pointer(id);

    m_assert(ptr != NULL, "No control bound to id %" PRIu32, id);

    return *ptr;
  }

  Control& ControlBinding::get (char const* name) const {
    Control* ptr = get_pointer(name);

    m_assert(ptr != NULL, "No control bound to name '%s'", name);

    return *ptr;
  }


  InputCombination& ControlBinding::operator [] (u32_t id) const {
    return get(id).input_combination;
  }

  InputCombination& ControlBinding::operator [] (char const* name) const {
    return get(name).input_combination;
  }


  ArrayIterator<Control> ControlBinding::begin () const {
    return controls.begin();
  }

  ArrayIterator<Control> ControlBinding::end () const {
    return controls.end();
  }


  void ControlBinding::set (u32_t id, InputCombination const& input_combination) {
    Control& control = get(id);

    control.input_combination = input_combination;
  }

  u32_t ControlBinding::set (char const* name, InputCombination const& input_combination) {
    Control* existing_control = get_pointer(name);
    
    u32_t id;

    if (existing_control != NULL) {
      id = existing_control->id;

      existing_control->input_combination = input_combination;
    } else {
      id = control_id_counter;
      
      Control new_control = {
        id,
        name,
        input_combination
      };

      ++ control_id_counter;

      controls.append(new_control);
    }

    return id;
  }

  u32_t ControlBinding::set (Control const& prototype) {
    Control* existing_control = get_pointer(prototype.name);
    
    u32_t id;

    if (existing_control != NULL) {
      id = existing_control->id;

      *existing_control = { id, prototype };
    } else {
      id = control_id_counter;
      
      Control new_control = { id, prototype };

      ++ control_id_counter;

      controls.append(new_control);
    }

    return id;
  }


  void ControlBinding::unset (u32_t id) {
    Control* control = get_pointer(id);
    
    if (control != NULL) {
      control->destroy();
      controls.remove(controls.get_index(control));
    }
  }

  void ControlBinding::unset (char const* name) {
    Control* control = get_pointer(name);

    if (control != NULL) {
      control->destroy();
      controls.remove(controls.get_index(control));
    }
  }

  void ControlBinding::clear () {
    for (auto [ i, control ] : controls) control.active = false;
  }

  void ControlBinding::destroy () {
    for (auto [ i, control ] : controls) control.destroy();
    controls.destroy();
  }



  
  JSONItem Input::to_json_item () const {
    return control_binding.to_json_item();
  }

  s64_t Input::get_control_index (u32_t id) const {
    return control_binding.get_index(id);
  }

  s64_t Input::get_control_index (char const* name) const {
    return control_binding.get_index(name);
  }

  Control* Input::get_control_pointer (u32_t id) const {
    return control_binding.get_pointer(id);
  }
  
  Control* Input::get_control_pointer (char const* name) const {
    return control_binding.get_pointer(name);
  }
  

  Control& Input::get_control (u32_t id) const {
    return control_binding.get(id);
  }
  
  Control& Input::get_control (char const* name) const {
    return control_binding.get(name);
  }
  


  void Input::bind (u32_t id, InputCombination const& input_combination) {
    return control_binding.set(id, input_combination);
  }
  
  u32_t Input::bind (char const* name, InputCombination const& input_combination) {
    return control_binding.set(name, input_combination);
  }
  
  u32_t Input::bind (Control const& control) {
    return control_binding.set(control);
  }
  

  void Input::unbind (u32_t id) {
    return control_binding.unset(id);
  }
  
  void Input::unbind (char const* name) {
    return control_binding.unset(name);
  }
  


  void Input::show_binding_menu () {
    static String control_string;

    using namespace ImGui;

    Columns(2, "Input Binding Columns");
    for (auto [ i, control ] : control_binding) {
      control_string.clear();
      control.input_combination.generate_string(control_string);

      Text("%s", control.name);
      NextColumn();
      if (Button(control_string.value, { GetContentRegionAvailWidth(), 0 })) {
        begin_capture_binding(control.id);
      }
      NextColumn();
    }
    Columns(1);
  }

  void Input::show_binding_modal () const {
    static String control_string;
    
    using namespace ImGui;

    if (BeginPopupModal("ModEngine Input Binding Capture Modal", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs)) {
      if (capture_id == 0) CloseCurrentPopup();
      else {
        Text("Binding Control %s (Press escape to cancel)", get_control(capture_id).name);

        control_string.clear();
        capture_combo.generate_string(control_string);

        Text("to: %s", control_string.value);
      }
      
      EndPopup();
    }
  }


  void Input::begin_capture_binding (u32_t id) {
    m_assert(get_control_index(id) != -1, "Cannot capture binding for undefined Control %" PRIu32, id);
    clear();
    capture_combo.clear();
    capture_id = id;
    capturing_binding = true;
    ImGui::OpenPopup("ModEngine Input Binding Capture Modal");
  }


  void Input::process_sdl_input (SDL_Event const& event) {
    raw.process_sdl_input(event);
  }

  bool Input::process_binding_capture_step () {
    static KeyboardInput::KeyMask modifier_mask = ~(KeyboardInput::KeyMask { Keycode::ShiftL, Keycode::ShiftR, Keycode::CtrlL, Keycode::CtrlR, Keycode::AltL, Keycode::AltR });

    if (capturing_binding) {
      if (raw.keyboard.active_keys[Keycode::Escape]) {
        capture_id = 0;
        capturing_binding = false;
        raw.clear();
        return false;
      } else if (!capture_combo.test(raw)) {
        control_binding[capture_id] = capture_combo;
        capture_id = 0;
        capturing_binding = false;
        raw.clear();
      } else {
        capture_combo.modifiers |= raw.keyboard.active_modifiers;
        capture_combo.keys |= raw.keyboard.active_keys & modifier_mask;
        capture_combo.buttons |= raw.mouse.active_buttons;
      }

      return false;
    } else {
      return true;
    }
  }

  void Input::begin_frame () {
    raw.begin_frame();
  }

  void Input::process_raw_input (bool in_mouse_usable, Vector2s const& app_resolution) {
    if (process_binding_capture_step()) {
      mouse_position_px = raw.mouse.position;
      
      mouse_usable = in_mouse_usable;

      if (mouse_usable) {
        mouse_position_unit = {
          (2.0f * mouse_position_px.x) / app_resolution.x - 1.0f,
          1.0f - (2.0f * mouse_position_px.y) / app_resolution.y
        };
      } else {
        mouse_position_unit = { -2, -2 };
      }

      for (auto [ i, control ] : control_binding) {
        control.active = control.test(raw);
      }
    }
  }


  bool Input::get (u32_t id) const {
    return get_control(id).active;
  }

  bool Input::get (char const* name) const {
    return get_control(name).active;
  }


  bool& Input::operator [] (u32_t id) const {
    return get_control(id).active;
  }

  bool& Input::operator [] (char const* name) const {
    return get_control(name).active;
  }


  void Input::set (u32_t id) const {
    get_control(id).active = true;
  }

  void Input::set (char const* name) const {
    get_control(name).active = true;
  }


  void Input::unset (u32_t id) const {
    get_control(id).active = false;
  }

  void Input::unset (char const* name) const {
    get_control(name).active = false;
  }


  void Input::clear () {
    control_binding.clear();
    raw.clear();
    
    mouse_position_px = { -1, -1 };
    mouse_position_unit = { -2, -2 };
    mouse_usable = false;
  }


  void Input::destroy () {
    control_binding.destroy();
  }
}