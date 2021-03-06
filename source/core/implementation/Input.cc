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


    if (modifiers.any_bits()) {
      JSONArray modifier_arr;

      for (u8_t modifier = 0; modifier < ModifierKey::total_modifier_count; modifier ++) {
        if (modifiers.match_index(modifier)) {
          modifier_arr.append({ ModifierKey::name(modifier) });
        }
      }

      object.set(modifier_arr, "modifiers");
    }

    if (keys.any_bits()) {
      JSONArray key_arr;

      for (u8_t key = 0; key < Keycode::total_key_count; key ++) {
        if (keys.match_index(key)) {
          key_arr.append({ Keycode::name(key) });
        }
      }

      object.set(key_arr, "keys");
    }


    if (buttons.any_bits()) {
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


  
  JSONObject ControlBinding::to_json_object () const {
    JSONObject object;
    
    for (auto [ i, control ] : controls) {
      object.set({ control.input_combination.to_json_item() }, control.name.value);
    }

    return object;
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
      if (str_cmp_caseless(name, control.name.value) == 0) return i;
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
      if (str_cmp_caseless(name, control.name.value) == 0) return &control;
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
      controls.remove(controls.get_index(control));
    }
  }

  void ControlBinding::unset (char const* name) {
    Control* control = get_pointer(name);

    if (control != NULL) {
      controls.remove(controls.get_index(control));
    }
  }

  void ControlBinding::clear () {
    for (auto [ i, control ] : controls) control.active = false;
  }

  void ControlBinding::destroy () {
    controls.destroy();
  }






  Input_t Input = { };

  char const* Input_t::config_path = "./assets/controls.json";

  Array<Control> Input_t::default_controls = Array<Control>::from_elements_static(
    Control { "Forward", { { }, { Keycode::W }, { } } },
    Control { "Left", { { }, { Keycode::A }, { } } },
    Control { "Backward", { { }, { Keycode::S }, { } } },
    Control { "Right", { { }, { Keycode::D }, { } } },
    Control { "Up", { { }, { Keycode::Space }, { } } },
    Control { "Down", { { }, { Keycode::C }, { } } },
    Control { "Primary Action", { { }, { }, { MouseButton::Left } } },
    Control { "Secondary Action", { { }, { }, { MouseButton::Right } } },
    Control { "Zoom In", { {}, {}, { MouseButton::WheelUp } } },
    Control { "Zoom Out", { {}, {}, { MouseButton::WheelDown } } }
  );


  Input_t& Input_t::init () {
    JSON json;

    try {
      json = JSON::from_file(config_path);
    } catch (Exception& exception) {
      exception.handle();
      json = { JSONType::Object, "Empty default config" };
    }

    try {
      if (json.get_object().items.count > 0) {
        for (auto [ i, control ] : default_controls) {
          JSONItem* control_item = json.get_object_item(control.name.value);

          if (control_item != NULL) {
            try {
              bind(control.name.value, InputCombination::from_json_item(*control_item));
            } catch (Exception& exception) {
              printf("Input config warning: ");
              exception.print();
              exception.handle();
              bind(control);
            }
          } else {
            bind(control);
          }
        }
      } else {
        for (auto [ i, control ] : default_controls) bind(control);
      }
    } catch (Exception& exception) {
      json.destroy();
      control_binding.destroy();
      throw exception;
    }

    json.destroy();

    return *this;
  }


  void Input_t::destroy () {
    JSON json = { to_json_object(), "New outbound Input config" };

    if (!json.to_file(config_path)) {
      printf("Warning: Failed to save Input config file at path '%s', make sure the containing folder exists\n", config_path);
    }

    json.destroy();

    control_binding.destroy();
  }


  
  JSONObject Input_t::to_json_object () const {
    return control_binding.to_json_object();
  }

  s64_t Input_t::get_control_index (u32_t id) const {
    return control_binding.get_index(id);
  }

  s64_t Input_t::get_control_index (char const* name) const {
    return control_binding.get_index(name);
  }

  Control* Input_t::get_control_pointer (u32_t id) const {
    return control_binding.get_pointer(id);
  }
  
  Control* Input_t::get_control_pointer (char const* name) const {
    return control_binding.get_pointer(name);
  }
  

  Control& Input_t::get_control (u32_t id) const {
    return control_binding.get(id);
  }
  
  Control& Input_t::get_control (char const* name) const {
    return control_binding.get(name);
  }
  


  void Input_t::bind (u32_t id, InputCombination const& input_combination) {
    return control_binding.set(id, input_combination);
  }
  
  u32_t Input_t::bind (char const* name, InputCombination const& input_combination) {
    return control_binding.set(name, input_combination);
  }
  
  u32_t Input_t::bind (Control const& control) {
    return control_binding.set(control);
  }
  

  void Input_t::unbind (u32_t id) {
    return control_binding.unset(id);
  }
  
  void Input_t::unbind (char const* name) {
    return control_binding.unset(name);
  }
  


  void Input_t::show_binding_menu () {
    static String control_string = String { 0, true };

    using namespace ImGui;

    Columns(2, "Input Binding Columns");
    for (auto [ i, control ] : control_binding) {
      control_string.clear();
      control.input_combination.generate_string(control_string);

      Text("%s", control.name.value);
      NextColumn();
      if (Button(control_string.value, { GetContentRegionAvailWidth(), 0 })) {
        begin_capture_binding(control.id);
      }
      NextColumn();
    }
    Columns(1);
  }

  void Input_t::show_binding_modal () const {
    static String control_string = String { 0, true };
    
    using namespace ImGui;

    if (BeginPopupModal("ModEngine Input Binding Capture Modal", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs)) {
      if (capture_id == 0) CloseCurrentPopup();
      else {
        Text("Binding Control %s (Press escape to cancel)", get_control(capture_id).name.value);

        control_string.clear();
        capture_combo.generate_string(control_string);

        Text("to: %s", control_string.value);
      }
      
      EndPopup();
    }
  }


  void Input_t::begin_capture_binding (u32_t id) {
    m_assert(get_control_index(id) != -1, "Cannot capture binding for undefined Control %" PRIu32, id);
    clear();
    capture_combo.clear();
    capture_id = id;
    capturing_binding = true;
    ImGui::OpenPopup("ModEngine Input Binding Capture Modal");
  }


  void Input_t::process_sdl_input (SDL_Event const& event) {
    raw.process_sdl_input(event);
  }

  bool Input_t::process_binding_capture_step () {
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

  void Input_t::begin_frame () {
    raw.begin_frame();
  }

  void Input_t::process_raw_input (bool in_mouse_usable, Vector2s const& app_resolution) {
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


  bool Input_t::get (u32_t id) const {
    return get_control(id).active;
  }

  bool Input_t::get (char const* name) const {
    return get_control(name).active;
  }


  bool& Input_t::operator [] (u32_t id) const {
    return get_control(id).active;
  }

  bool& Input_t::operator [] (char const* name) const {
    return get_control(name).active;
  }


  void Input_t::set (u32_t id) const {
    get_control(id).active = true;
  }

  void Input_t::set (char const* name) const {
    get_control(name).active = true;
  }


  void Input_t::unset (u32_t id) const {
    get_control(id).active = false;
  }

  void Input_t::unset (char const* name) const {
    get_control(name).active = false;
  }


  void Input_t::clear () {
    control_binding.clear();
    raw.clear();
    
    mouse_position_px = { -1, -1 };
    mouse_position_unit = { -2, -2 };
    mouse_usable = false;
  }
}