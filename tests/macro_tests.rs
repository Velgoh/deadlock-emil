use deadlock_macro::config::MacroSettings;
use deadlock_macro::engine::MacroEngine;
use deadlock_macro::gif_player::{GifPlayer, EMBEDDED_ZERO_GIF};
use deadlock_macro::input::{InputManager, MacroKeyStep};
use std::fs;
use std::time::Instant;
use windows_sys::Win32::UI::Input::KeyboardAndMouse::{
    VK_DOWN, VK_ESCAPE, VK_F1, VK_F12, VK_F24, VK_LBUTTON, VK_LCONTROL, VK_LEFT, VK_LSHIFT,
    VK_MBUTTON, VK_NUMPAD1, VK_NUMPAD5, VK_RBUTTON, VK_RIGHT, VK_SPACE, VK_TAB, VK_UP,
    VK_XBUTTON1, VK_XBUTTON2,
};

// Test 1: User default combo sequence parsing
#[test]
fn test_user_default_sequence_parsing() {
    let seq = "2, 2, z, z, 2, 2, x, x, 3, 3";
    let mut steps = Vec::new();
    let mut err = String::new();

    let ok = InputManager::parse_sequence(seq, &mut steps, &mut err, false);
    assert!(ok, "Failed to parse default sequence: {}", err);
    assert_eq!(steps.len(), 10, "Expected 10 steps, got {}", steps.len());

    assert_eq!(steps[0].label, "2");
    assert_eq!(steps[0].vk, b'2' as u16);
    assert_eq!(steps[1].label, "2");
    assert_eq!(steps[1].vk, b'2' as u16);
    assert_eq!(steps[2].label, "Z");
    assert_eq!(steps[2].vk, b'Z' as u16);
    assert_eq!(steps[3].label, "Z");
    assert_eq!(steps[3].vk, b'Z' as u16);
    assert_eq!(steps[4].label, "2");
    assert_eq!(steps[4].vk, b'2' as u16);
    assert_eq!(steps[5].label, "2");
    assert_eq!(steps[5].vk, b'2' as u16);
    assert_eq!(steps[6].label, "X");
    assert_eq!(steps[6].vk, b'X' as u16);
    assert_eq!(steps[7].label, "X");
    assert_eq!(steps[7].vk, b'X' as u16);
    assert_eq!(steps[8].label, "3");
    assert_eq!(steps[8].vk, b'3' as u16);
    assert_eq!(steps[9].label, "3");
    assert_eq!(steps[9].vk, b'3' as u16);

    for (i, step) in steps.iter().enumerate() {
        assert!(step.scan_code > 0, "Step {} scancode must be non-zero", i);
    }
}

// Test 2: Delimiter variations (spaces, semicolons, arrows, pipes)
#[test]
fn test_delimiter_variations() {
    let mut steps = Vec::new();
    let mut err = String::new();

    // Space separated
    assert!(InputManager::parse_sequence("2 2 z z 2 2 x x 3 3", &mut steps, &mut err, false));
    assert_eq!(steps.len(), 10);

    // Semicolon separated
    assert!(InputManager::parse_sequence("2;2;z;z;2;2;x;x;3;3", &mut steps, &mut err, false));
    assert_eq!(steps.len(), 10);

    // Arrow syntax "2 -> 2 -> z -> z"
    assert!(InputManager::parse_sequence("2 -> 2 -> z -> z -> 2 -> 2 -> x -> x -> 3 -> 3", &mut steps, &mut err, false));
    assert_eq!(steps.len(), 10);

    // Pipe separated
    assert!(InputManager::parse_sequence("2|2|z|z|2|2|x|x|3|3", &mut steps, &mut err, false));
    assert_eq!(steps.len(), 10);
}

// Test 3: Named and special keys
#[test]
fn test_named_keys() {
    let mut steps = Vec::new();
    let mut err = String::new();

    let seq = "SPACE LSHIFT LCTRL TAB ESC F1 F12";
    assert!(InputManager::parse_sequence(seq, &mut steps, &mut err, false), "{}", err);
    assert_eq!(steps.len(), 7);

    assert_eq!(steps[0].vk, VK_SPACE);
    assert_eq!(steps[1].vk, VK_LSHIFT);
    assert_eq!(steps[2].vk, VK_LCONTROL);
    assert_eq!(steps[3].vk, VK_TAB);
    assert_eq!(steps[4].vk, VK_ESCAPE);
    assert_eq!(steps[5].vk, VK_F1);
    assert_eq!(steps[6].vk, VK_F12);
}

// Test 4: Extended keys and mouse button token parsing
#[test]
fn test_extended_and_mouse_input_parsing() {
    let mut steps = Vec::new();
    let mut err = String::new();

    let seq = "LMB RMB MMB M1 M2 M3 M4 M5 UP DOWN LEFT RIGHT NUM1 NUMPAD5 [ ]";
    assert!(InputManager::parse_sequence(seq, &mut steps, &mut err, false), "{}", err);
    assert_eq!(steps.len(), 16);

    // Mouse
    assert!(steps[0].is_mouse && steps[0].vk == VK_LBUTTON);
    assert!(steps[1].is_mouse && steps[1].vk == VK_RBUTTON);
    assert!(steps[2].is_mouse && steps[2].vk == VK_MBUTTON);
    assert!(steps[3].is_mouse && steps[3].vk == VK_LBUTTON);
    assert!(steps[4].is_mouse && steps[4].vk == VK_RBUTTON);
    assert!(steps[5].is_mouse && steps[5].vk == VK_MBUTTON);
    assert!(steps[6].is_mouse && steps[6].vk == VK_XBUTTON1);
    assert!(steps[7].is_mouse && steps[7].vk == VK_XBUTTON2);

    // Arrows
    assert!(steps[8].is_extended && steps[8].vk == VK_UP);
    assert!(steps[9].is_extended && steps[9].vk == VK_DOWN);
    assert!(steps[10].is_extended && steps[10].vk == VK_LEFT);
    assert!(steps[11].is_extended && steps[11].vk == VK_RIGHT);

    // Numpad
    assert_eq!(steps[12].vk, VK_NUMPAD1);
    assert_eq!(steps[13].vk, VK_NUMPAD5);

    // Symbols
    assert_eq!(steps[14].label, "[");
    assert_eq!(steps[15].label, "]");
}

// Test 5: Invalid and edge-case inputs
#[test]
fn test_invalid_inputs() {
    let mut steps = Vec::new();
    let mut err = String::new();

    assert!(!InputManager::parse_sequence("", &mut steps, &mut err, false));
    assert!(!err.is_empty());

    assert!(!InputManager::parse_sequence("   \t  \r\n  ", &mut steps, &mut err, false));

    assert!(!InputManager::parse_sequence("2, 2, INVALID_UNKNOWN_KEY, 3", &mut steps, &mut err, false));
    assert!(err.contains("INVALID_UNKNOWN_KEY"));
}

// Test 6: Hardware ScanCode mapping accuracy for Deadlock / Source 2
#[test]
fn test_scancode_mapping() {
    let mut step = MacroKeyStep {
        label: String::new(),
        vk: 0,
        scan_code: 0,
        is_extended: false,
        is_mouse: false,
        mouse_flags_down: 0,
        mouse_flags_up: 0,
    };

    assert!(InputManager::parse_key_token("2", &mut step));
    assert_eq!(step.scan_code, 0x03, "Scan code for '2' should be 0x03");

    assert!(InputManager::parse_key_token("3", &mut step));
    assert_eq!(step.scan_code, 0x04, "Scan code for '3' should be 0x04");

    assert!(InputManager::parse_key_token("z", &mut step));
    assert_eq!(step.scan_code, 0x2C, "Scan code for 'z' should be 0x2C");

    assert!(InputManager::parse_key_token("x", &mut step));
    assert_eq!(step.scan_code, 0x2D, "Scan code for 'x' should be 0x2D");
}

// Test 7: Configuration persistence (Save and Load INI)
#[test]
fn test_config_persistence() {
    let test_path = "test_settings_temp.ini";

    let mut s1 = MacroSettings::default();
    s1.sequence_string = "2, 2, z, z, 2, 2, x, x, 3, 3".to_string();
    s1.trigger_key_vk = VK_XBUTTON1 as u32;
    s1.toggle_hotkey_vk = 0x75; // F6
    s1.press_delay_ms = 500;
    s1.key_hold_duration_ms = 45;
    s1.double_tap_delay_ms = 70;
    s1.use_fast_double_tap = true;
    s1.jitter_ms = 30;
    s1.loop_delay_ms = 0;
    s1.input_mode = 0;
    s1.sound_beep_on_toggle = false;
    s1.macro_enabled = true;
    s1.auto_double_tap = true;

    assert!(s1.save_to_file(test_path), "Failed to save test config");

    let mut s2 = MacroSettings::default();
    assert!(s2.load_from_file(test_path), "Failed to load test config");

    assert_eq!(s2.sequence_string, s1.sequence_string);
    assert_eq!(s2.trigger_key_vk, s1.trigger_key_vk);
    assert_eq!(s2.toggle_hotkey_vk, s1.toggle_hotkey_vk);
    assert_eq!(s2.press_delay_ms, 500);
    assert_eq!(s2.key_hold_duration_ms, 45);
    assert_eq!(s2.double_tap_delay_ms, 70);
    assert_eq!(s2.use_fast_double_tap, true);
    assert_eq!(s2.jitter_ms, 30);
    assert_eq!(s2.loop_delay_ms, 0);
    assert_eq!(s2.input_mode, 0);
    assert_eq!(s2.sound_beep_on_toggle, false);
    assert_eq!(s2.macro_enabled, true);
    assert_eq!(s2.auto_double_tap, true);

    let _ = fs::remove_file(test_path);
}

// Test 8: Clamping and boundary validation
#[test]
fn test_config_bounds_clamping() {
    let mut s = MacroSettings::default();
    s.press_delay_ms = 5;
    s.key_hold_duration_ms = 1;
    s.jitter_ms = 500;
    s.input_mode = 99;
    s.sequence_string = "".to_string();

    s.validate_and_clamp();

    assert!(s.press_delay_ms >= 20, "press_delay_ms should be clamped to min 20");
    assert!(s.key_hold_duration_ms >= 5, "key_hold_duration_ms should be clamped to min 5");
    assert!(s.jitter_ms <= 200, "jitter_ms should be clamped to max 200");
    assert_eq!(s.input_mode, 0, "invalid input_mode should reset to 0");
    assert!(!s.sequence_string.is_empty(), "empty sequence should reset to default sequence");
}

// Test 9: Real abort latency verification (< 25ms mid-sequence interrupt)
#[test]
fn test_immediate_abort_latency() {
    let mut settings = MacroSettings::default();
    settings.sequence_string = "2, 2, z, z, 2, 2, x, x, 3, 3".to_string();
    settings.press_delay_ms = 500;
    settings.macro_enabled = true;
    settings.trigger_key_vk = VK_F24 as u32; // Unpressed trigger key

    let engine = MacroEngine::new(settings);

    let start = Instant::now();
    let completed = engine.interruptible_sleep_for_test(500);
    let elapsed_ms = start.elapsed().as_millis();

    assert!(!completed, "interruptible_sleep must return false when trigger is not held");
    assert!(elapsed_ms < 25, "Abort latency must be under 25ms, got {}ms", elapsed_ms);

    engine.set_macro_enabled(false);
    assert!(!engine.is_macro_enabled());

    engine.shutdown();
    assert_eq!(InputManager::get_active_tracked_keys_count(), 0);
}

// Test 10: Key tracking and safety panic release
#[test]
fn test_tracked_keys_safety_release() {
    let step = MacroKeyStep {
        label: "2".to_string(),
        vk: b'2' as u16,
        scan_code: 0x03,
        is_extended: false,
        is_mouse: false,
        mouse_flags_down: 0,
        mouse_flags_up: 0,
    };

    InputManager::send_key_down(&step, 0);
    assert_eq!(InputManager::get_active_tracked_keys_count(), 1);

    InputManager::release_all_simulated_keys(0);
    assert_eq!(InputManager::get_active_tracked_keys_count(), 0);
}

// Test 11: Shorthand input without delimiters ("123zc" -> 1,1,2,2,3,3,z,z,c,c)
#[test]
fn test_shorthand_no_delimiters() {
    let mut steps = Vec::new();
    let mut err = String::new();

    let ok = InputManager::parse_sequence("123zc", &mut steps, &mut err, true);
    assert!(ok, "{}", err);
    assert_eq!(steps.len(), 10);

    assert_eq!(steps[0].label, "1");
    assert_eq!(steps[1].label, "1");
    assert_eq!(steps[2].label, "2");
    assert_eq!(steps[3].label, "2");
    assert_eq!(steps[4].label, "3");
    assert_eq!(steps[5].label, "3");
    assert_eq!(steps[6].label, "Z");
    assert_eq!(steps[7].label, "Z");
    assert_eq!(steps[8].label, "C");
    assert_eq!(steps[9].label, "C");
}

// Test 12: Shorthand with delimiters auto-doubled ("1,2,3,z,c")
#[test]
fn test_shorthand_with_delimiters() {
    let mut steps = Vec::new();
    let mut err = String::new();

    let ok = InputManager::parse_sequence("1,2,3,z,c", &mut steps, &mut err, true);
    assert!(ok, "{}", err);
    assert_eq!(steps.len(), 10);

    assert_eq!(steps[0].vk, b'1' as u16);
    assert_eq!(steps[1].vk, b'1' as u16);
    assert_eq!(steps[2].vk, b'2' as u16);
    assert_eq!(steps[3].vk, b'2' as u16);
    assert_eq!(steps[4].vk, b'3' as u16);
    assert_eq!(steps[5].vk, b'3' as u16);
    assert_eq!(steps[6].vk, b'Z' as u16);
    assert_eq!(steps[7].vk, b'Z' as u16);
    assert_eq!(steps[8].vk, b'C' as u16);
    assert_eq!(steps[9].vk, b'C' as u16);
}

// Test 13: Already doubled keys must NOT quadruple-expand
#[test]
fn test_already_doubled_no_quadruple() {
    let mut steps = Vec::new();
    let mut err = String::new();

    let ok = InputManager::parse_sequence("1, 1, 2, 2, 3, 3, z, z, c, c", &mut steps, &mut err, true);
    assert!(ok, "{}", err);
    assert_eq!(steps.len(), 10, "Expected 10 steps (not quadruple expanded 20)");

    let mut steps2 = Vec::new();
    let ok2 = InputManager::parse_sequence("112233zzcc", &mut steps2, &mut err, true);
    assert!(ok2, "{}", err);
    assert_eq!(steps2.len(), 10);
}

// Test 14: Auto double-tap disabled returns single sequential presses
#[test]
fn test_auto_double_tap_disabled() {
    let mut steps = Vec::new();
    let mut err = String::new();

    let ok = InputManager::parse_sequence("123zc", &mut steps, &mut err, false);
    assert!(ok, "{}", err);
    assert_eq!(steps.len(), 5);

    assert_eq!(steps[0].vk, b'1' as u16);
    assert_eq!(steps[1].vk, b'2' as u16);
    assert_eq!(steps[2].vk, b'3' as u16);
    assert_eq!(steps[3].vk, b'Z' as u16);
    assert_eq!(steps[4].vk, b'C' as u16);
}

// Test 15: Mixed single and already doubled keys
#[test]
fn test_mixed_shorthand_and_doubled() {
    let mut steps = Vec::new();
    let mut err = String::new();

    let ok = InputManager::parse_sequence("1, 2, 2, 3", &mut steps, &mut err, true);
    assert!(ok, "{}", err);
    assert_eq!(steps.len(), 6);

    assert_eq!(steps[0].vk, b'1' as u16);
    assert_eq!(steps[1].vk, b'1' as u16);
    assert_eq!(steps[2].vk, b'2' as u16);
    assert_eq!(steps[3].vk, b'2' as u16);
    assert_eq!(steps[4].vk, b'3' as u16);
    assert_eq!(steps[5].vk, b'3' as u16);
}

// Test 16: Compound shorthand with named keys
#[test]
fn test_shorthand_with_named_keys() {
    let mut steps = Vec::new();
    let mut err = String::new();

    let ok = InputManager::parse_sequence("123 SPACE zc", &mut steps, &mut err, true);
    assert!(ok, "{}", err);
    assert_eq!(steps.len(), 12);

    assert_eq!(steps[0].vk, b'1' as u16);
    assert_eq!(steps[1].vk, b'1' as u16);
    assert_eq!(steps[2].vk, b'2' as u16);
    assert_eq!(steps[3].vk, b'2' as u16);
    assert_eq!(steps[4].vk, b'3' as u16);
    assert_eq!(steps[5].vk, b'3' as u16);
    assert_eq!(steps[6].vk, VK_SPACE);
    assert_eq!(steps[7].vk, VK_SPACE);
    assert_eq!(steps[8].vk, b'Z' as u16);
    assert_eq!(steps[9].vk, b'Z' as u16);
    assert_eq!(steps[10].vk, b'C' as u16);
    assert_eq!(steps[11].vk, b'C' as u16);
}

// Test 17: Function key with trailing characters ("F1Z" decomposes into F1 and Z)
#[test]
fn test_function_key_trailing_chars() {
    let mut step = MacroKeyStep {
        label: String::new(),
        vk: 0,
        scan_code: 0,
        is_extended: false,
        is_mouse: false,
        mouse_flags_down: 0,
        mouse_flags_up: 0,
    };

    assert!(!InputManager::parse_key_token("F1Z", &mut step));

    let mut steps = Vec::new();
    let mut err = String::new();
    assert!(InputManager::parse_sequence("F1Z", &mut steps, &mut err, false));
    assert_eq!(steps.len(), 2);
    assert_eq!(steps[0].vk, VK_F1);
    assert_eq!(steps[1].vk, b'Z' as u16);
}

// Test 18: Shorthand with printable symbols and brackets ("123[c")
#[test]
fn test_shorthand_with_symbols() {
    let mut steps = Vec::new();
    let mut err = String::new();

    assert!(InputManager::parse_sequence("123[c", &mut steps, &mut err, true));
    assert_eq!(steps.len(), 10);
    assert_eq!(steps[0].vk, b'1' as u16);
    assert_eq!(steps[1].vk, b'1' as u16);
    assert_eq!(steps[2].vk, b'2' as u16);
    assert_eq!(steps[3].vk, b'2' as u16);
    assert_eq!(steps[4].vk, b'3' as u16);
    assert_eq!(steps[5].vk, b'3' as u16);
    assert_eq!(steps[6].label, "[");
    assert_eq!(steps[7].label, "[");
    assert_eq!(steps[8].vk, b'C' as u16);
    assert_eq!(steps[9].vk, b'C' as u16);
}

// Test 19: GIF decoding and anti-aliased corner rounding mask
#[test]
fn test_gif_decoding_and_corner_rounding() {
    let player = GifPlayer::load_from_memory(EMBEDDED_ZERO_GIF, 8.0).expect("Failed to decode zero.gif");
    assert_eq!(player.width, 540);
    assert_eq!(player.height, 227);

    // Verify corner pixels are completely transparent (alpha = 0)
    let first_frame = &player.frames[0].image;
    // Top-left corner (0, 0)
    assert_eq!(first_frame.pixels[0].a(), 0, "Top-left corner pixel must be transparent");
    // Top-right corner (width - 1, 0)
    assert_eq!(first_frame.pixels[player.width - 1].a(), 0, "Top-right corner pixel must be transparent");
    // Bottom-left corner (0, height - 1)
    let bl_idx = (player.height - 1) * player.width;
    assert_eq!(first_frame.pixels[bl_idx].a(), 0, "Bottom-left corner pixel must be transparent");
    // Bottom-right corner (width - 1, height - 1)
    let br_idx = player.height * player.width - 1;
    assert_eq!(first_frame.pixels[br_idx].a(), 0, "Bottom-right corner pixel must be transparent");

    // Center pixel should remain fully opaque
    let center_idx = (player.height / 2) * player.width + (player.width / 2);
    assert_eq!(first_frame.pixels[center_idx].a(), 255, "Center pixel must be fully opaque");
}

