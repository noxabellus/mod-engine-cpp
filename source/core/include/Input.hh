#ifndef INPUT_H
#define INPUT_H

#include "cstd.hh"
#include "util.hh"
#include "Array.hh"
#include "String.hh"
#include "Bitmask.hh"
#include "math/Vector2.hh"



namespace mod {
  namespace Keycode {
    enum: u8_t {
      Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,

      A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

      BackSlash,
      ForwardSlash,
      Comma,
      Tilde,
      BracketL,
      BracketR,
      Minus,
      Equal,
      Period,
      Semicolon,
      Apostrophe,

      Space,
      Tab,

      Up, Down, Left, Right,

      Escape,
      Backspace,
      Return,
      PageUp,
      PageDown,
      Pause,
      PrintScreen,

      CapsLock,
      NumLock,
      ScrollLock,

      ShiftL,
      ShiftR,
      CtrlL,
      CtrlR,
      AltL,
      AltR,
      
      LeftGUI,
      RightGUI,

      F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
      F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,

      Keypad0, Keypad00, Keypad000, Keypad1, Keypad2, Keypad3, Keypad4, Keypad5, Keypad6, Keypad7, Keypad8, Keypad9,
      KeypadA, KeypadB, KeypadC, KeypadD, KeypadE, KeypadF,
      KeypadAmp, KeypadAt, KeypadColon, KeypadComma, KeypadEqual, KeypadEqualAS400, KeypadExclamation, KeypadGreater,
      KeypadHash, KeypadBraceL, KeypadBraceR, KeypadParenL, KeypadParenR, KeypadLesser, KeypadPeriod, 
      KeypadSpace, KeypadTab,
      KeypadPlus, KeypadMinus, KeypadMultiply, KeypadDivide, KeypadPercent, KeypadPlusMinus, KeypadPower, KeypadOr, KeypadXor,
      KeypadDoubleAmp, KeypadDoubleOr, 
      KeypadBackspace, KeypadEnter,
      KeypadBinary, KeypadDecimal, KeypadHex, KeypadOctal,
      KeypadMemAdd, KeypadMemSub, KeypadMemMul, KeypadMemDiv, KeypadMemClear, KeypadMemStore, KeypadMemRecall,
      KeypadClear,
      KeypadClearEntry,

      Copy,
      Cut,
      Paste,
      Delete,

      ACBack,
      ACForward,
      ACBookmarks,
      ACHome,
      ACRefresh,
      ACSearch,
      ACStop,

      Again,
      AltErase,

      Application,

      AudioMute,
      AudioNext,
      AudioPlay,
      AudioPrev,
      AudioStop,
      AudioRewind,
      AudioFastForward,

      BrightnessDown,
      BrightnessUp,

      Calculator,

      Cancel,

      Clear,
      ClearAgain,

      Computer,
      CrSel,

      CurrencySubUnit,
      CurrencyUnit,
      
      DecimalSeparator,
      DisplaySwitch,

      Eject,

      Execute,
      ExSel,
      Find,
      Help,
      Home,

      KeyIlluminationDown,
      KeyIlluminationToggle,
      KeyIlluminationUp,

      Mail,
      MediaSelect,
      Menu,
      Mode,
      Mute,

      Oper,
      Out,
      Power,

      Prior,
      Select,
      Separator,
      Sleep,
      Stop,
      SysReq,

      ThousandsSeparator,

      Undo,

      VolumeUp,
      VolumeDown,

      WWW,


      total_key_count,

      Invalid = (u8_t) -1
    };

    static constexpr char const* names [total_key_count] = {
      "Zero", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine",

      "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",

      "BackSlash",
      "ForwardSlash",
      "Comma",
      "Tilde",
      "BracketL",
      "BracketR",
      "Minus",
      "Equal",
      "Period",
      "Semicolon",
      "Apostrophe",

      "Space",
      "Tab",

      "Up", "Down", "Left", "Right",

      "Escape",
      "Backspace",
      "Return",
      "PageUp",
      "PageDown",
      "Pause",
      "PrintScreen",

      "CapsLock",
      "NumLock",
      "ScrollLock",
      "ShiftL",
      "ShiftR",
      "CtrlL",
      "CtrlR",
      "AltL",
      "AltR",
      "LeftGUI",
      "RightGUI",

      "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
      "F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20", "F21", "F22", "F23", "F24",

      "Keypad0", "Keypad00", "Keypad000", "Keypad1", "Keypad2", "Keypad3", "Keypad4", "Keypad5", "Keypad6", "Keypad7", "Keypad8", "Keypad9",
      "KeypadA", "KeypadB", "KeypadC", "KeypadD", "KeypadE", "KeypadF",
      "KeypadAmp", "KeypadAt", "KeypadColon", "KeypadComma", "KeypadEqual", "KeypadEqualAS400", "KeypadExclamation", "KeypadGreater",
      "KeypadHash", "KeypadBraceL", "KeypadBraceR", "KeypadParenL", "KeypadParenR", "KeypadLesser", "KeypadPeriod", 
      "KeypadSpace", "KeypadTab",
      "KeypadPlus", "KeypadMinus", "KeypadMultiply", "KeypadDivide", "KeypadPercent", "KeypadPlusMinus", "KeypadPower", "KeypadOr", "KeypadXor",
      "KeypadDoubleAmp", "KeypadDoubleOr", 
      "KeypadBackspace", "KeypadEnter",
      "KeypadBinary", "KeypadDecimal", "KeypadHex", "KeypadOctal",
      "KeypadMemAdd", "KeypadMemSub", "KeypadMemMul", "KeypadMemDiv", "KeypadMemClear", "KeypadMemStore", "KeypadMemRecall",
      "KeypadClear",
      "KeypadClearEntry",

      "Copy",
      "Cut",
      "Paste",
      "Delete",

      "ACBack",
      "ACForward",
      "ACBookmarks",
      "ACHome",
      "ACRefresh",
      "ACSearch",
      "ACStop",

      "Again",
      "AltErase",

      "Application",

      "AudioMute",
      "AudioNext",
      "AudioPlay",
      "AudioPrev",
      "AudioStop",
      "AudioRewind",
      "AudioFastForward",

      "BrightnessDown",
      "BrightnessUp",

      "Calculator",

      "Cancel",

      "Clear",
      "ClearAgain",

      "Computer",
      "CrSel",

      "CurrencySubUnit",
      "CurrencyUnit",
      
      "DecimalSeparator",
      "DisplaySwitch",

      "Eject",

      "Execute",
      "ExSel",
      "Find",
      "Help",
      "Home",

      "KeyIlluminationDown",
      "KeyIlluminationToggle",
      "KeyIlluminationUp",

      "Mail",
      "MediaSelect",
      "Menu",
      "Mode",
      "Mute",

      "Oper",
      "Out",
      "Power",

      "Prior",
      "Select",
      "Separator",
      "Sleep",
      "Stop",
      "SysReq",

      "ThousandsSeparator",

      "Undo",

      "VolumeUp",
      "VolumeDown",

      "WWW"
    };

    /* Get the name of a Keycode as a str */
    static constexpr char const* name (u8_t code) {
      if (code < total_key_count) return names[code];
      else return "Invalid";
    }

    /* Get a Keycode from its name in str form */
    static constexpr u8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      for (size_t code = 0; code < total_key_count; code ++) {
        if (str_cmp_caseless(name, names[code], max_length) == 0) return code;
      }

      return Invalid;
    }

    static constexpr SDL_Keycode sdl_versions [total_key_count] = {
      SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9,

      SDLK_a, SDLK_b, SDLK_c, SDLK_d, SDLK_e, SDLK_f, SDLK_g, SDLK_h, SDLK_i, SDLK_j, SDLK_k, SDLK_l, SDLK_m, SDLK_n, SDLK_o, SDLK_p, SDLK_q, SDLK_r, SDLK_s, SDLK_t, SDLK_u, SDLK_v, SDLK_w, SDLK_x, SDLK_y, SDLK_z,

      SDLK_BACKSLASH,
      SDLK_SLASH,
      SDLK_COMMA,
      SDLK_BACKQUOTE,
      SDLK_LEFTBRACKET,
      SDLK_RIGHTBRACKET,
      SDLK_MINUS,
      SDLK_EQUALS,
      SDLK_PERIOD,
      SDLK_SEMICOLON,
      SDLK_QUOTE,

      SDLK_SPACE,
      SDLK_TAB,

      SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT,

      SDLK_ESCAPE,
      SDLK_BACKSPACE,
      SDLK_RETURN,
      SDLK_PAGEUP,
      SDLK_PAGEDOWN,
      SDLK_PAUSE,
      SDLK_PRINTSCREEN,

      SDLK_CAPSLOCK,
      SDLK_NUMLOCKCLEAR,
      SDLK_SCROLLLOCK,
      SDLK_LSHIFT,
      SDLK_RSHIFT,
      SDLK_LCTRL,
      SDLK_RCTRL,
      SDLK_LALT,
      SDLK_RALT,
      SDLK_LGUI,
      SDLK_RGUI,

      SDLK_F1, SDLK_F2, SDLK_F3, SDLK_F4, SDLK_F5, SDLK_F6, SDLK_F7, SDLK_F8, SDLK_F9, SDLK_F10, SDLK_F11, SDLK_F12,
      SDLK_F13, SDLK_F14, SDLK_F15, SDLK_F16, SDLK_F17, SDLK_F18, SDLK_F19, SDLK_F20, SDLK_F21, SDLK_F22, SDLK_F23, SDLK_F24,

      SDLK_KP_0, SDLK_KP_00, SDLK_KP_000, SDLK_KP_1, SDLK_KP_2, SDLK_KP_3, SDLK_KP_4, SDLK_KP_5, SDLK_KP_6, SDLK_KP_7, SDLK_KP_8, SDLK_KP_9,
      SDLK_KP_A, SDLK_KP_B, SDLK_KP_C, SDLK_KP_D, SDLK_KP_E, SDLK_KP_F,
      SDLK_KP_AMPERSAND, SDLK_KP_AT, SDLK_KP_COLON, SDLK_KP_COMMA, SDLK_KP_EQUALS, SDLK_KP_EQUALSAS400, SDLK_KP_EXCLAM, SDLK_KP_GREATER,
      SDLK_KP_HASH, SDLK_KP_LEFTBRACE, SDLK_KP_RIGHTBRACE, SDLK_KP_LEFTPAREN, SDLK_KP_RIGHTPAREN, SDLK_KP_LESS, SDLK_KP_PERIOD, 
      SDLK_KP_SPACE, SDLK_KP_TAB,
      SDLK_KP_PLUS, SDLK_KP_MINUS, SDLK_KP_MULTIPLY, SDLK_KP_DIVIDE, SDLK_KP_PERCENT, SDLK_KP_PLUSMINUS, SDLK_KP_POWER, SDLK_KP_VERTICALBAR, SDLK_KP_XOR,
      SDLK_KP_DBLAMPERSAND, SDLK_KP_DBLVERTICALBAR, 
      SDLK_KP_BACKSPACE, SDLK_KP_ENTER,
      SDLK_KP_BINARY, SDLK_KP_DECIMAL, SDLK_KP_HEXADECIMAL, SDLK_KP_OCTAL,
      SDLK_KP_MEMADD, SDLK_KP_MEMSUBTRACT, SDLK_KP_MEMMULTIPLY, SDLK_KP_MEMDIVIDE, SDLK_KP_MEMCLEAR, SDLK_KP_MEMSTORE, SDLK_KP_MEMRECALL,
      SDLK_KP_CLEAR,
      SDLK_KP_CLEARENTRY,

      SDLK_COPY,
      SDLK_CUT,
      SDLK_PASTE,
      SDLK_DELETE,

      SDLK_AC_BACK,
      SDLK_AC_FORWARD,
      SDLK_AC_BOOKMARKS,
      SDLK_AC_HOME,
      SDLK_AC_REFRESH,
      SDLK_AC_REFRESH,
      SDLK_AC_STOP,

      SDLK_AGAIN,
      SDLK_ALTERASE,

      SDLK_APPLICATION,

      SDLK_AUDIOMUTE,
      SDLK_AUDIONEXT,
      SDLK_AUDIOPLAY,
      SDLK_AUDIOPREV,
      SDLK_AUDIOSTOP,
      SDLK_AUDIOREWIND,
      SDLK_AUDIOFASTFORWARD,

      SDLK_BRIGHTNESSDOWN,
      SDLK_BRIGHTNESSUP,

      SDLK_CALCULATOR,

      SDLK_CANCEL,

      SDLK_CLEAR,
      SDLK_CLEARAGAIN,

      SDLK_COMPUTER,
      SDLK_CRSEL,

      SDLK_CURRENCYSUBUNIT,
      SDLK_CURRENCYUNIT,
      
      SDLK_DECIMALSEPARATOR,
      SDLK_DISPLAYSWITCH,

      SDLK_EJECT,

      SDLK_EXECUTE,
      SDLK_EXSEL,
      SDLK_FIND,
      SDLK_HELP,
      SDLK_HOME,

      SDLK_KBDILLUMDOWN,
      SDLK_KBDILLUMDOWN,
      SDLK_KBDILLUMUP,

      SDLK_MAIL,
      SDLK_MEDIASELECT,
      SDLK_MENU,
      SDLK_MODE,
      SDLK_MUTE,

      SDLK_OPER,
      SDLK_OUT,
      SDLK_POWER,

      SDLK_PRIOR,
      SDLK_SELECT,
      SDLK_SEPARATOR,
      SDLK_SLEEP,
      SDLK_STOP,
      SDLK_SYSREQ,

      SDLK_THOUSANDSSEPARATOR,

      SDLK_UNDO,

      SDLK_VOLUMEUP,
      SDLK_VOLUMEDOWN,

      SDLK_WWW
    };
    
    /* Get an SDL_Keycode from a Keycode.
     * Returns SDLK_UNKNOWN if the Keycode was invalid */
    static constexpr SDL_Keycode to_sdl (u8_t code) {
      if (code < total_key_count) return sdl_versions[code];
      else return SDLK_UNKNOWN;
    }

    /* Get a Keycode from an SDL_Keycode */
    static constexpr u8_t from_sdl (SDL_Keycode sdl_code) {
      for (u8_t code = 0; code < total_key_count; code ++ ) {
        if (sdl_versions[code] == sdl_code) return code;
      }

      return Invalid;
    }

    /* Determine if a value is a valid Keycode */
    static constexpr bool validate (u8_t code) {
      return code < total_key_count;
    }

    static constexpr char char_versions [Up] = {
      '0','1','2','3','4','5','6','7','8','9',
      'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
      
      '\\',
      '/',
      ',',
      '`',
      '[',
      ']',
      '-',
      '=',
      '.',
      ';',
      '\'',

      ' ',
      '\t'
    };

    /* Determine if a value is a Keycode representable by a visual character */
    static constexpr bool validate_typable_char (u8_t code) {
      return code < Up;
    }

    /* Get the visual character version of a Keycode
     * Returns the null character if the Keycode was invalid */
    static constexpr char to_char (u8_t code) {
      if (validate_typable_char(code)) return char_versions[code];
      else return '\0';
    }

    /* Get a Keycode from a visual character.
     * Returns Invalid if the visual character was not typable under normal circumstances, or was invalid itself
     * (E.g. backspace, though it does have a character representation, is not allowed) */
    static constexpr u8_t from_char (char ch) {
      for (u8_t code = 0; code < Up; code ++) {
        if (char_versions[code] == char_to_lower(ch)) return code;
      }

      return Invalid;
    }
  }


  namespace LockKey {
    enum: u8_t {
      Caps,
      Num,
      Scroll,

      total_lock_count,

      Invalid = (u8_t) -1
    };

    static constexpr char const* names [total_lock_count] = {
      "Caps",
      "Num",
      "Scroll"
    };

    /* Get the name of a LockKey as a str */
    static constexpr char const* name (u8_t lock) {
      if (lock < total_lock_count) return names[lock];
      else return "Invalid";
    }

    /* Get a LockKey from its name in str form */
    static constexpr u8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      for (u8_t lock = 0; lock < total_lock_count; lock ++) {
        if (str_cmp_caseless(name, names[lock], max_length) == 0) return lock;
      }

      return Invalid;
    }

    static constexpr u8_t keycode_versions [total_lock_count] = {
      Keycode::CapsLock,
      Keycode::NumLock,
      Keycode::ScrollLock,
    };
    
    /* Get a LockKey from a Keycode */
    static constexpr u8_t from_keycode (u8_t keycode) {
      for (u8_t lock = 0; lock < total_lock_count; lock ++) {
        if (keycode_versions[lock] == keycode) return lock;
      }

      return Invalid;
    }

    /* Get the Keycode version of a LockKey.
     * Returns KeyCode::Invalid if the LockKey was invalid */
    static constexpr u8_t to_keycode (u8_t lock) {
      if (lock < total_lock_count) return keycode_versions[lock];
      else return 0;
    }

    /* Determine if a value is a valid LockKey */
    static constexpr bool validate (u8_t lock) {
      return lock < total_lock_count;
    }
    

    /* Determine if a value is a Keycode that corresponds to a LockKey */
    static constexpr bool validate_key (u8_t code) {
      return code == Keycode::CapsLock
          || code == Keycode::NumLock
          || code == Keycode::ScrollLock;
    }
  };


  namespace ModifierKey {
    enum: u8_t {
      Shift,
      Ctrl,
      Alt,

      total_modifier_count,

      Invalid = (u8_t) -1
    };

    static constexpr char const* names [total_modifier_count] = {
      "Shift",
      "Ctrl",
      "Alt"
    };

    /* Get the name of a ModifierKey as a str */
    static constexpr char const* name (u8_t modifier) {
      if (modifier < total_modifier_count) return names[modifier];
      else return "Invalid";
    }

    /* Get a ModifierKey from its name in str form */
    static constexpr u8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      for (u8_t modifier = 0; modifier < total_modifier_count; modifier ++) {
        if (str_cmp_caseless(name, names[modifier], max_length) == 0) return modifier;
      }

      return Invalid;
    }

    static constexpr u8_t keycode_versions [total_modifier_count] = {
      Keycode::ShiftL,
      Keycode::CtrlL,
      Keycode::AltL
    };
    
    /* Get a ModifierKey from a Keycode */
    static constexpr u8_t from_keycode (s32_t keycode) {
      for (u8_t modifier = 0; modifier < total_modifier_count; modifier ++) {
        if (keycode_versions[modifier] == keycode || keycode_versions[modifier] == keycode - 1) return modifier;
      }

      return Invalid;
    }

    /* Get the Keycode version of a ModifierKey.
     * Handededness determined by optional bool parameter (default, false, is left, true is right)
     * Returns KeyCode::Invalid if the ModifierKey was invalid */
    static constexpr s32_t to_keycode (u8_t modifier, bool right_hand = false) {
      if (modifier < total_modifier_count) return right_hand? keycode_versions[modifier] + 1 : keycode_versions[modifier];
      else return Keycode::Invalid;
    }

    /* Determine if a value is a valid ModifierKey */
    static constexpr bool validate (u8_t modifier) {
      return modifier < total_modifier_count;
    }

    /* Determine if a value is a Keycode that corresponds to a ModifierKey */
    static constexpr bool validate_key (u8_t code) {
      return code >= Keycode::ShiftL
          && code <= Keycode::AltR;
    }
  };


  namespace MouseButton {
    enum: u8_t {
      Left, Middle, Right,
      Back, Forward,

      WheelUp, WheelDown,
      WheelLeft, WheelRight,

      total_button_count,
      
      Invalid = (u8_t) -1
    };

    static constexpr char const* names [total_button_count] = {
      "Left", "Middle", "Right",
      "Back", "Forward",
      "WheelUp", "WheelDown",
      "WheelLeft", "WheelRight"
    };

    /* Get the name of a MouseButton as a str */
    static constexpr char const* name (u8_t button) {
      if (button < total_button_count) return names[button];
      else return "Invalid";
    }

    /* Get a MouseButton from its name in str form */
    static constexpr u8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      for (u8_t button = 0; button < total_button_count; button ++) {
        if (str_cmp_caseless(name, names[button], max_length) == 0) return button;
      }

      return Invalid;
    }

    static constexpr s32_t sdl_versions [WheelUp] = {
      SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE, SDL_BUTTON_RIGHT,
      SDL_BUTTON_X1, SDL_BUTTON_X2
    };
    
    /* Get a MouseButton from an SDL_BUTTON */
    static constexpr u8_t from_sdl (s32_t sdl_button) {
      for (u8_t button = 0; button < WheelUp; button ++) {
        if (sdl_versions[button] == sdl_button) return button;
      }

      return Invalid;
    }

    /* Get the SDL_BUTTON version of a MouseButton.
     * Returns 0 if the MouseButton was invalid or had no SDL equivalent
     * (Wheel events are no longer handled as buttons in SDL) */
    static constexpr s32_t to_sdl (u8_t button) {
      if (button < WheelUp) return sdl_versions[button];
      else return 0;
    }

    /* Determine if a value is a valid MouseButton */
    static constexpr bool validate (u8_t button) {
      return button < total_button_count;
    }

    /* Determine if a value is a real MouseButton
     * (E.g. returns true if value is not a "Wheel" button) */
    static constexpr bool validate_physical (u8_t button) {
      return button < WheelUp;
    }
  };



  struct KeyboardInput {
    using KeyMask = Bitmask<224>;
    using LockMask = Bitmask<8>;
    using ModifierMask = Bitmask<8>;

    KeyMask active_keys = { };
    LockMask active_locks = { };
    ModifierMask active_modifiers = { };


    /* Update KeyboardInput state from an SDL_Event */
    ENGINE_API void process_sdl_input (SDL_Event const& event);


    /* Get the status of a particular KeyboardInput key */
    ENGINE_API bool operator [] (u8_t code) const;
    
    /* Get the status of a particular KeyboardInput key by name */
    ENGINE_API bool operator [] (char const* name) const;

    /* Get the status of a particular KeyboardInput key by its equivalent typed character */
    ENGINE_API bool char_key (char ch) const;
    
    
    /* Get the status of a particular LockKey */
    ENGINE_API bool lock (u8_t lock) const;
    
    /* Get the status of a particular LockKey by name */
    ENGINE_API bool lock (char const* name) const;


    /* Get the status of a particular ModifierKey */
    ENGINE_API bool modifier (u8_t modifier) const;

    /* Get the status of a particular ModifierKey by name */
    ENGINE_API bool modifier (char const* name) const;


    /* Clear all key, lock and modifier state of a KeyboardInput */
    ENGINE_API void clear ();
  };


  struct MouseInput {
    using ButtonMask = Bitmask<16>;

    ButtonMask active_buttons = { };
    Vector2s position = { 0, 0 };


    /* Create a new zero-initialized MouseInput */
    MouseInput () { }

    /* Create a MouseInput and poll SDL for the position of the mouse */
    ENGINE_API static MouseInput from_sdl_state ();

    /* Clear state data to begin a new frame (This is to simulate button presses for wheel) */
    ENGINE_API void begin_frame ();

    /* Update MouseInput state from an SDL_Event */
    ENGINE_API void process_sdl_input (SDL_Event const& event);

    /* Get the status of a particular MouseInput button */
    ENGINE_API bool operator [] (u8_t button) const;

    /* Get the status of a particular MouseInput button by name */
    ENGINE_API bool operator [] (char const* name) const;

    
    /* Clear all button and position state of a MouseInput */
    ENGINE_API void clear ();
  };


  struct RawInput {
    KeyboardInput keyboard;
    MouseInput mouse;


    /* Create a new zero-initialized RawInput state set */
    RawInput () { }

    /* Create a new RawInput state set initialized by polling sdl for MouseState */
    ENGINE_API static RawInput from_sdl_state ();

    /* Start a new frame of input */
    ENGINE_API void begin_frame ();

    /* Update RawInput state from an SDL_Event */
    ENGINE_API void process_sdl_input (SDL_Event const& event);


    /* Clear all state of an RawInput */
    ENGINE_API void clear ();
  };



  struct InputCombination {
    KeyboardInput::ModifierMask modifiers;
    KeyboardInput::KeyMask keys;
    MouseInput::ButtonMask buttons;


    /* Determine if an InputCombination is a subset of the active items of a RawInput */
    ENGINE_API bool test (RawInput const& raw_input) const;

    /* Append all the input items names of an InputCombination to a String with + separators if needed */
    ENGINE_API void generate_string (String& out) const;

    /* Clear the selected input items of an InputCombination (This will cause it to activate all the time if not rebound) */
    ENGINE_API void clear ();
  };
  


  struct Control {
    u32_t id;

    char* name;

    InputCombination input_combination;

    bool active = false;


    Control ();

    Control (
      u32_t in_id,
      char const* in_name,
      InputCombination const& in_input_combination
    )
    : id(in_id)
    , name(str_clone(in_name))
    , input_combination(in_input_combination)
    { }

    Control (
      char const* in_name,
      InputCombination const& in_input_combination
    )
    : id(0)
    , name(str_clone(in_name))
    , input_combination(in_input_combination)
    { }

    Control (
      u32_t in_id,
      Control const& proto
    )
    : id(in_id)
    , name(str_clone(proto.name))
    , input_combination(proto.input_combination)
    { }


    /* Determine if a Control's InputCombination is a subset of a RawInput's active input items */
    ENGINE_API bool test (RawInput const& raw_input) const;

    /* Clear a Controls' InputCombination */
    ENGINE_API void clear ();

    /* Free the heap allocation of a Control */
    ENGINE_API void destroy ();
  };


  struct ControlBinding {
    Array<Control> controls;
    u32_t control_id_counter = 1;
    

    /* Get the index of a Control in a ControlBinding by id */
    ENGINE_API s64_t get_index (u32_t id) const;

    /* Get the index of a Control in a ControlBinding by name */
    ENGINE_API s64_t get_index (char const* name) const;


    /* Get a pointer to a Control in a ControlBinding by id */
    ENGINE_API Control* get_pointer (u32_t id) const;

    /* Get a pointer to a Control in a ControlBinding by name */
    ENGINE_API Control* get_pointer (char const* name) const;


    /* Get a Control in a ControlBinding by id */
    ENGINE_API Control& get (u32_t id) const;

    /* get a Control in a ControlBinding by name */
    ENGINE_API Control& get (char const* name) const;


    /* Get the InputCombination of a Control in a ControlBinding by id */
    ENGINE_API InputCombination& operator [] (u32_t id) const;

    /* Get the InputCombination of a Control in a ControlBinding by name */
    ENGINE_API InputCombination& operator [] (char const* name) const;


    /* Get an iterator at the start of the Controls in a ControlBinding */
    ENGINE_API ArrayIterator<Control> begin () const;

    /* Get an iterator at the end of the Controls in a ControlBinding */
    ENGINE_API ArrayIterator<Control> end () const;


    /* Set the InputCombination of a Control in a ControlBinding by id
     * Panics if the id does not match any Control */
    ENGINE_API void set (u32_t id, InputCombination const& input_combination);

    /* Set the InputCombination of a Control in a ControlBinding by name and return the id.
     * Creates a new Control if none exists */
    ENGINE_API u32_t set (char const* name, InputCombination const& input_combination);

    /* Set the InputCombination of a Control in a ControlBinding by copying a prototypical Control.
     * Creates a new Control if none exists */
    ENGINE_API u32_t set (Control const& prototype);


    /* Remove a Control from a ControlBinding by id
     * Does nothing if no Control has the given id */
    ENGINE_API void unset (u32_t id);

    /* Remove a Control from a ControlBinding by name
     * Does nothing if no Control has the given name */
    ENGINE_API void unset (char const* name);


    /* Clear the active state of all Controls in a ControlBinding */
    ENGINE_API void clear ();

    /* Destroy the heap allocations of a ControlBinding and its Controls */
    ENGINE_API void destroy ();
  };


  struct Input {
    ControlBinding control_binding;
    RawInput raw;


    Vector2s mouse_position = { 0, 0 };

    InputCombination capture_combo;
    s64_t capture_id = 0;
    bool capturing_binding = false;


    /* Get the index of a Control in the ControlBinding of an Input, by id */
    ENGINE_API s64_t get_control_index (u32_t id) const;

    /* Get the index of a Control in the ControlBinding of an Input, by name */
    ENGINE_API s64_t get_control_index (char const* name) const;


    /* Get a pointer to a Control in the ControlBinding of an Input, by id */
    ENGINE_API Control* get_control_pointer (u32_t id) const;

    /* Get a pointer to a Control in the ControlBinding of an Input, by name */
    ENGINE_API Control* get_control_pointer (char const* name) const;


    /* Get a Control in the ControlBinding of an Input, by id */
    ENGINE_API Control& get_control (u32_t id) const;

    /* Get a Control in the ControlBinding of an Input, by name */
    ENGINE_API Control& get_control (char const* name) const;



    /* Bind the InputCombination of a Control in the ControlBinding of an Input, by id.
     * Panics if no Control has the given id */
    ENGINE_API void bind (u32_t id, InputCombination const& input_combination);

    /* Bind the InputCombination of a Control in the ControlBinding of an Input, by name.
     * Creates a new Control, if no Control has the given name */
    ENGINE_API u32_t bind (char const* name, InputCombination const& input_combination);

    /* Bind the InputCombination of a Control in the ControlBinding of an Input, by copying from a prototypical Control.
     * Creates a new Control if no Control matches the name of the prototype */
    ENGINE_API u32_t bind (Control const& control);


    /* Remove a Control from the ControlBinding of an Input, by id.
     * Does nothing if no Control has the given id */
    ENGINE_API void unbind (u32_t id);

    /* Remove a Control from the ControlBinding of an Input, by name.
     * Does nothing if no Control has the given name */
    ENGINE_API void unbind (char const* name);


    /* Display a series of ImGui elements that allow rebinding of all Controls in the ControlBinding of an Input */
    ENGINE_API void show_binding_menu ();

    /* Display an ImGui overlay showing the current combination of inputs being bound to a Control in the ControlBinding of an Input.
     * This should be called always below show_binding_menu (Within the same ImGui Window) and will only show the modal when appropriate  */
    ENGINE_API void show_binding_modal () const;


    /* Begins binding a new InputCombination for a Control in the ControlBinding of an Input.
     * This is used internally by show_binding_menu.
     * Panics if the id is invalid */
    ENGINE_API void begin_capture_binding (u32_t id);


    /* Update the internal RawInput of an Input by processing an SDL_Event */
    ENGINE_API void process_sdl_input (SDL_Event const& event);

    /* Used internally by process_raw_input to handle updating the binding InputCombination. */
    ENGINE_API bool process_binding_capture_step ();

    /* Clear an Input's mouse state for a new frame of input (Allows simulating wheel events as buttons) */
    ENGINE_API void begin_frame ();

    /* Use the internal RawInput to activate appropriate Controls in the ControlBinding of an Input */
    ENGINE_API void process_raw_input ();


    /* Get the status of a Control in the ControlBinding of an Input by id */
    ENGINE_API bool get (u32_t id) const;

    /* Get the status of a Control in the ControlBinding of an Input by name */
    ENGINE_API bool get (char const* name) const;


    /* Get a reference to the status of a Control in the ControlBinding of an Input by id */
    ENGINE_API bool& operator [] (u32_t id) const;

    /* Get a reference to the status of a Control in the ControlBinding of an Input by name */
    ENGINE_API bool& operator [] (char const* name) const;


    /* Activate a Control in the ControlBinding of an Input by id */
    ENGINE_API void set (u32_t id) const;

    /* Activate a Control in the ControlBinding of an Input by name */
    ENGINE_API void set (char const* name) const;


    /* Deactivate a Control in the ControlBinding of an Input by id */
    ENGINE_API void unset (u32_t id) const;

    /* Deactivate a Control in the ControlBinding of an Input by name */
    ENGINE_API void unset (char const* name) const;


    /* Clear all state data of an Input */
    ENGINE_API void clear ();


    /* Clean up the heap allocations of an Input */
    ENGINE_API void destroy ();
  };
}

#endif