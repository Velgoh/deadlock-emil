use std::sync::Mutex;
use windows_sys::Win32::UI::Input::KeyboardAndMouse::{
    GetAsyncKeyState, MapVirtualKeyW, SendInput, VkKeyScanW, INPUT, INPUT_KEYBOARD,
    INPUT_MOUSE, KEYBDINPUT, KEYEVENTF_EXTENDEDKEY, KEYEVENTF_KEYUP, KEYEVENTF_SCANCODE,
    MAPVK_VK_TO_VSC, MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP, MOUSEEVENTF_MIDDLEDOWN,
    MOUSEEVENTF_MIDDLEUP, MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP, MOUSEEVENTF_XDOWN,
    MOUSEEVENTF_XUP, MOUSEINPUT, VK_BACK, VK_CAPITAL, VK_CONTROL, VK_DELETE, VK_DOWN, VK_END,
    VK_ESCAPE, VK_F1, VK_HOME, VK_INSERT, VK_LBUTTON, VK_LCONTROL, VK_LEFT, VK_LMENU,
    VK_LSHIFT, VK_MBUTTON, VK_MENU, VK_NEXT, VK_NUMPAD0, VK_PRIOR, VK_RBUTTON,
    VK_RCONTROL, VK_RETURN, VK_RIGHT, VK_RMENU, VK_RSHIFT, VK_SHIFT, VK_SPACE, VK_TAB, VK_UP,
    VK_XBUTTON1, VK_XBUTTON2,
};

const XBUTTON1: u32 = 0x0001;
const XBUTTON2: u32 = 0x0002;

#[derive(Clone, Debug, PartialEq)]
pub struct MacroKeyStep {
    pub label: String,
    pub vk: u16,
    pub scan_code: u16,
    pub is_extended: bool,
    pub is_mouse: bool,
    pub mouse_flags_down: u32,
    pub mouse_flags_up: u32,
}

#[derive(Clone, Copy, Debug, PartialEq)]
struct TrackedKey {
    vk: u16,
    scan_code: u16,
    is_extended: bool,
    is_mouse: bool,
    mouse_flags_up: u32,
}

static TRACKED_KEYS: Mutex<Vec<TrackedKey>> = Mutex::new(Vec::new());

pub struct InputManager;

impl InputManager {
    pub fn parse_key_token(raw_token: &str, out_step: &mut MacroKeyStep) -> bool {
        let token = raw_token.trim().to_uppercase();
        if token.is_empty() {
            return false;
        }

        out_step.label = token.clone();
        out_step.is_extended = false;
        out_step.is_mouse = false;
        out_step.mouse_flags_down = 0;
        out_step.mouse_flags_up = 0;

        // Mouse buttons
        match token.as_str() {
            "LMB" | "MOUSE1" | "LBUTTON" => {
                out_step.vk = VK_LBUTTON;
                out_step.is_mouse = true;
                out_step.mouse_flags_down = MOUSEEVENTF_LEFTDOWN;
                out_step.mouse_flags_up = MOUSEEVENTF_LEFTUP;
                return true;
            }
            "RMB" | "MOUSE2" | "RBUTTON" => {
                out_step.vk = VK_RBUTTON;
                out_step.is_mouse = true;
                out_step.mouse_flags_down = MOUSEEVENTF_RIGHTDOWN;
                out_step.mouse_flags_up = MOUSEEVENTF_RIGHTUP;
                return true;
            }
            "MMB" | "MOUSE3" | "MBUTTON" => {
                out_step.vk = VK_MBUTTON;
                out_step.is_mouse = true;
                out_step.mouse_flags_down = MOUSEEVENTF_MIDDLEDOWN;
                out_step.mouse_flags_up = MOUSEEVENTF_MIDDLEUP;
                return true;
            }
            "M4" | "MOUSE4" | "XBUTTON1" => {
                out_step.vk = VK_XBUTTON1;
                out_step.is_mouse = true;
                out_step.mouse_flags_down = MOUSEEVENTF_XDOWN;
                out_step.mouse_flags_up = MOUSEEVENTF_XUP;
                return true;
            }
            "M5" | "MOUSE5" | "XBUTTON2" => {
                out_step.vk = VK_XBUTTON2;
                out_step.is_mouse = true;
                out_step.mouse_flags_down = MOUSEEVENTF_XDOWN;
                out_step.mouse_flags_up = MOUSEEVENTF_XUP;
                return true;
            }
            _ => {}
        }

        // Single character (alphanumeric & printable ASCII symbols)
        if token.len() == 1 {
            let c = token.chars().next().unwrap();
            if c.is_ascii_digit() || c.is_ascii_uppercase() {
                out_step.vk = c as u16;
                out_step.scan_code = unsafe { MapVirtualKeyW(out_step.vk as u32, MAPVK_VK_TO_VSC) as u16 };
                return true;
            }

            let vk_res = unsafe { VkKeyScanW(c as u16) };
            if vk_res != -1 {
                out_step.vk = (vk_res & 0xFF) as u16;
                out_step.scan_code = unsafe { MapVirtualKeyW(out_step.vk as u32, MAPVK_VK_TO_VSC) as u16 };
                return true;
            }
        }

        // Arrow keys
        match token.as_str() {
            "UP" => {
                out_step.vk = VK_UP;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_UP as u32, MAPVK_VK_TO_VSC) as u16 };
                out_step.is_extended = true;
                return true;
            }
            "DOWN" => {
                out_step.vk = VK_DOWN;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_DOWN as u32, MAPVK_VK_TO_VSC) as u16 };
                out_step.is_extended = true;
                return true;
            }
            "LEFT" => {
                out_step.vk = VK_LEFT;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_LEFT as u32, MAPVK_VK_TO_VSC) as u16 };
                out_step.is_extended = true;
                return true;
            }
            "RIGHT" => {
                out_step.vk = VK_RIGHT;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_RIGHT as u32, MAPVK_VK_TO_VSC) as u16 };
                out_step.is_extended = true;
                return true;
            }
            _ => {}
        }

        // Common named keys
        match token.as_str() {
            "SPACE" => {
                out_step.vk = VK_SPACE;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_SPACE as u32, MAPVK_VK_TO_VSC) as u16 };
                return true;
            }
            "SHIFT" | "LSHIFT" => {
                out_step.vk = VK_LSHIFT;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_SHIFT as u32, MAPVK_VK_TO_VSC) as u16 };
                return true;
            }
            "RSHIFT" => {
                out_step.vk = VK_RSHIFT;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_RSHIFT as u32, MAPVK_VK_TO_VSC) as u16 };
                return true;
            }
            "CTRL" | "LCTRL" => {
                out_step.vk = VK_LCONTROL;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_CONTROL as u32, MAPVK_VK_TO_VSC) as u16 };
                return true;
            }
            "RCTRL" => {
                out_step.vk = VK_RCONTROL;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_CONTROL as u32, MAPVK_VK_TO_VSC) as u16 };
                out_step.is_extended = true;
                return true;
            }
            "ALT" | "LALT" => {
                out_step.vk = VK_LMENU;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_MENU as u32, MAPVK_VK_TO_VSC) as u16 };
                return true;
            }
            "RALT" => {
                out_step.vk = VK_RMENU;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_MENU as u32, MAPVK_VK_TO_VSC) as u16 };
                out_step.is_extended = true;
                return true;
            }
            "TAB" => {
                out_step.vk = VK_TAB;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_TAB as u32, MAPVK_VK_TO_VSC) as u16 };
                return true;
            }
            "ENTER" | "RETURN" => {
                out_step.vk = VK_RETURN;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_RETURN as u32, MAPVK_VK_TO_VSC) as u16 };
                return true;
            }
            "ESC" | "ESCAPE" => {
                out_step.vk = VK_ESCAPE;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_ESCAPE as u32, MAPVK_VK_TO_VSC) as u16 };
                return true;
            }
            "CAPS" | "CAPSLOCK" => {
                out_step.vk = VK_CAPITAL;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_CAPITAL as u32, MAPVK_VK_TO_VSC) as u16 };
                return true;
            }
            "BACKSPACE" | "BKSP" => {
                out_step.vk = VK_BACK;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_BACK as u32, MAPVK_VK_TO_VSC) as u16 };
                return true;
            }
            "DELETE" | "DEL" => {
                out_step.vk = VK_DELETE;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_DELETE as u32, MAPVK_VK_TO_VSC) as u16 };
                out_step.is_extended = true;
                return true;
            }
            "INSERT" | "INS" => {
                out_step.vk = VK_INSERT;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_INSERT as u32, MAPVK_VK_TO_VSC) as u16 };
                out_step.is_extended = true;
                return true;
            }
            "HOME" => {
                out_step.vk = VK_HOME;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_HOME as u32, MAPVK_VK_TO_VSC) as u16 };
                out_step.is_extended = true;
                return true;
            }
            "END" => {
                out_step.vk = VK_END;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_END as u32, MAPVK_VK_TO_VSC) as u16 };
                out_step.is_extended = true;
                return true;
            }
            "PGUP" | "PAGEUP" => {
                out_step.vk = VK_PRIOR;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_PRIOR as u32, MAPVK_VK_TO_VSC) as u16 };
                out_step.is_extended = true;
                return true;
            }
            "PGDN" | "PAGEDOWN" => {
                out_step.vk = VK_NEXT;
                out_step.scan_code = unsafe { MapVirtualKeyW(VK_NEXT as u32, MAPVK_VK_TO_VSC) as u16 };
                out_step.is_extended = true;
                return true;
            }
            _ => {}
        }

        // Numpad 0-9
        if token.starts_with("NUMPAD") && token.len() == 7 {
            if let Some(digit) = token.chars().nth(6).and_then(|c| c.to_digit(10)) {
                out_step.vk = VK_NUMPAD0 + digit as u16;
                out_step.scan_code = unsafe { MapVirtualKeyW(out_step.vk as u32, MAPVK_VK_TO_VSC) as u16 };
                return true;
            }
        }
        if token.starts_with("NUM") && token.len() == 4 {
            if let Some(digit) = token.chars().nth(3).and_then(|c| c.to_digit(10)) {
                out_step.vk = VK_NUMPAD0 + digit as u16;
                out_step.scan_code = unsafe { MapVirtualKeyW(out_step.vk as u32, MAPVK_VK_TO_VSC) as u16 };
                return true;
            }
        }

        // Function keys F1 - F24
        if token.starts_with('F') && token.len() >= 2 {
            let num_str = &token[1..];
            if num_str.chars().all(|c| c.is_ascii_digit()) {
                if let Ok(f_num) = num_str.parse::<u16>() {
                    if (1..=24).contains(&f_num) {
                        out_step.vk = VK_F1 + (f_num - 1);
                        out_step.scan_code = unsafe { MapVirtualKeyW(out_step.vk as u32, MAPVK_VK_TO_VSC) as u16 };
                        return true;
                    }
                }
            }
        }

        false
    }

    pub fn parse_sequence(
        sequence_str: &str,
        out_steps: &mut Vec<MacroKeyStep>,
        out_error: &mut String,
        auto_double_tap: bool,
    ) -> bool {
        out_steps.clear();
        out_error.clear();

        let trimmed = sequence_str.trim();
        if trimmed.is_empty() {
            *out_error = "Sequence string is empty".to_string();
            return false;
        }

        // Replace separators (comma, semicolon, pipe, arrow "->") with spaces
        let mut cleaned = String::with_capacity(sequence_str.len());
        let chars: Vec<char> = sequence_str.chars().collect();
        let mut i = 0;
        while i < chars.len() {
            if chars[i] == ',' || chars[i] == ';' || chars[i] == '|' {
                cleaned.push(' ');
                i += 1;
            } else if chars[i] == '-' && i + 1 < chars.len() && chars[i + 1] == '>' {
                cleaned.push(' ');
                i += 2;
            } else {
                cleaned.push(chars[i]);
                i += 1;
            }
        }

        let mut raw_steps = Vec::new();

        for token in cleaned.split_whitespace() {
            let mut single_step = MacroKeyStep {
                label: String::new(),
                vk: 0,
                scan_code: 0,
                is_extended: false,
                is_mouse: false,
                mouse_flags_down: 0,
                mouse_flags_up: 0,
            };

            // 1. Direct match as single token
            if Self::parse_key_token(token, &mut single_step) {
                raw_steps.push(single_step);
                continue;
            }

            // 2. Greedy decomposition for shorthand strings
            let mut pos = 0;
            let mut decomp_ok = true;
            let mut token_steps = Vec::new();

            while pos < token.len() {
                let mut matched_sub = false;

                // Try multi-character named keys (length 10 down to 2)
                let remaining = token.len() - pos;
                let max_len = 10.min(remaining);
                for len in (2..=max_len).rev() {
                    let sub = &token[pos..pos + len];
                    let mut sub_step = MacroKeyStep {
                        label: String::new(),
                        vk: 0,
                        scan_code: 0,
                        is_extended: false,
                        is_mouse: false,
                        mouse_flags_down: 0,
                        mouse_flags_up: 0,
                    };
                    if Self::parse_key_token(sub, &mut sub_step) {
                        token_steps.push(sub_step);
                        pos += len;
                        matched_sub = true;
                        break;
                    }
                }

                if matched_sub {
                    continue;
                }

                // Try single alphanumeric character or bracket symbol
                let c = token.as_bytes()[pos] as char;
                if c != '_' && (c.is_ascii_alphanumeric() || c == '[' || c == ']') {
                    let mut sub_step = MacroKeyStep {
                        label: String::new(),
                        vk: 0,
                        scan_code: 0,
                        is_extended: false,
                        is_mouse: false,
                        mouse_flags_down: 0,
                        mouse_flags_up: 0,
                    };
                    if Self::parse_key_token(&c.to_string(), &mut sub_step) {
                        token_steps.push(sub_step);
                        pos += 1;
                        matched_sub = true;
                    }
                }

                if !matched_sub {
                    decomp_ok = false;
                    break;
                }
            }

            if !decomp_ok || token_steps.is_empty() {
                *out_error = format!("Unrecognized key token: '{}'", token);
                return false;
            }

            raw_steps.extend(token_steps);
        }

        if raw_steps.is_empty() {
            *out_error = "Sequence contains no valid key commands".to_string();
            return false;
        }

        // Auto Double-Tap expansion
        if auto_double_tap {
            let mut idx = 0;
            while idx < raw_steps.len() {
                let mut is_already_doubled = false;
                if idx + 1 < raw_steps.len() {
                    let cur = &raw_steps[idx];
                    let next = &raw_steps[idx + 1];
                    if cur.vk == next.vk && cur.is_mouse == next.is_mouse && cur.scan_code == next.scan_code {
                        is_already_doubled = true;
                    }
                }

                if is_already_doubled {
                    out_steps.push(raw_steps[idx].clone());
                    out_steps.push(raw_steps[idx + 1].clone());
                    idx += 2;
                } else {
                    out_steps.push(raw_steps[idx].clone());
                    out_steps.push(raw_steps[idx].clone());
                    idx += 1;
                }
            }
        } else {
            *out_steps = raw_steps;
        }

        true
    }

    fn track_key(step: &MacroKeyStep, is_down: bool) {
        if let Ok(mut lock) = TRACKED_KEYS.lock() {
            if is_down {
                let exists = lock.iter().any(|k| {
                    k.vk == step.vk
                        && k.scan_code == step.scan_code
                        && k.is_extended == step.is_extended
                        && k.is_mouse == step.is_mouse
                });
                if !exists {
                    lock.push(TrackedKey {
                        vk: step.vk,
                        scan_code: step.scan_code,
                        is_extended: step.is_extended,
                        is_mouse: step.is_mouse,
                        mouse_flags_up: step.mouse_flags_up,
                    });
                }
            } else {
                if let Some(pos) = lock.iter().position(|k| {
                    k.vk == step.vk
                        && k.scan_code == step.scan_code
                        && k.is_extended == step.is_extended
                        && k.is_mouse == step.is_mouse
                }) {
                    lock.swap_remove(pos);
                }
            }
        }
    }

    pub fn get_active_tracked_keys_count() -> usize {
        TRACKED_KEYS.lock().map(|l| l.len()).unwrap_or(0)
    }

    pub fn send_key_down(step: &MacroKeyStep, input_mode: u32) {
        unsafe {
            if step.is_mouse {
                let mut input = std::mem::zeroed::<INPUT>();
                input.r#type = INPUT_MOUSE;
                input.Anonymous.mi = MOUSEINPUT {
                    dx: 0,
                    dy: 0,
                    mouseData: if step.mouse_flags_down == MOUSEEVENTF_XDOWN {
                        if step.vk == VK_XBUTTON1 {
                            XBUTTON1 as u32
                        } else {
                            XBUTTON2 as u32
                        }
                    } else {
                        0
                    },
                    dwFlags: step.mouse_flags_down,
                    time: 0,
                    dwExtraInfo: 0,
                };
                SendInput(1, &input, std::mem::size_of::<INPUT>() as i32);
            } else {
                let mut input = std::mem::zeroed::<INPUT>();
                input.r#type = INPUT_KEYBOARD;
                let mut flags = 0;

                if input_mode == 0 && step.scan_code > 0 {
                    // Hardware ScanCode (Best for Deadlock / Source 2)
                    input.Anonymous.ki = KEYBDINPUT {
                        wVk: 0,
                        wScan: step.scan_code,
                        dwFlags: KEYEVENTF_SCANCODE | if step.is_extended { KEYEVENTF_EXTENDEDKEY } else { 0 },
                        time: 0,
                        dwExtraInfo: 0,
                    };
                } else {
                    // Virtual Key fallback
                    if step.is_extended {
                        flags |= KEYEVENTF_EXTENDEDKEY;
                    }
                    input.Anonymous.ki = KEYBDINPUT {
                        wVk: step.vk,
                        wScan: step.scan_code,
                        dwFlags: flags,
                        time: 0,
                        dwExtraInfo: 0,
                    };
                }
                SendInput(1, &input, std::mem::size_of::<INPUT>() as i32);
            }
        }
        Self::track_key(step, true);
    }

    pub fn send_key_up(step: &MacroKeyStep, input_mode: u32) {
        unsafe {
            if step.is_mouse {
                let mut input = std::mem::zeroed::<INPUT>();
                input.r#type = INPUT_MOUSE;
                input.Anonymous.mi = MOUSEINPUT {
                    dx: 0,
                    dy: 0,
                    mouseData: if step.mouse_flags_up == MOUSEEVENTF_XUP {
                        if step.vk == VK_XBUTTON1 {
                            XBUTTON1 as u32
                        } else {
                            XBUTTON2 as u32
                        }
                    } else {
                        0
                    },
                    dwFlags: step.mouse_flags_up,
                    time: 0,
                    dwExtraInfo: 0,
                };
                SendInput(1, &input, std::mem::size_of::<INPUT>() as i32);
            } else {
                let mut input = std::mem::zeroed::<INPUT>();
                input.r#type = INPUT_KEYBOARD;

                if input_mode == 0 && step.scan_code > 0 {
                    input.Anonymous.ki = KEYBDINPUT {
                        wVk: 0,
                        wScan: step.scan_code,
                        dwFlags: KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP | if step.is_extended { KEYEVENTF_EXTENDEDKEY } else { 0 },
                        time: 0,
                        dwExtraInfo: 0,
                    };
                } else {
                    let mut flags = KEYEVENTF_KEYUP;
                    if step.is_extended {
                        flags |= KEYEVENTF_EXTENDEDKEY;
                    }
                    input.Anonymous.ki = KEYBDINPUT {
                        wVk: step.vk,
                        wScan: step.scan_code,
                        dwFlags: flags,
                        time: 0,
                        dwExtraInfo: 0,
                    };
                }
                SendInput(1, &input, std::mem::size_of::<INPUT>() as i32);
            }
        }
        Self::track_key(step, false);
    }

    pub fn release_all_simulated_keys(input_mode: u32) {
        let keys_to_release: Vec<TrackedKey> = {
            if let Ok(mut lock) = TRACKED_KEYS.lock() {
                let copy = lock.clone();
                lock.clear();
                copy
            } else {
                Vec::new()
            }
        };

        for key in keys_to_release {
            unsafe {
                if key.is_mouse {
                    let mut input = std::mem::zeroed::<INPUT>();
                    input.r#type = INPUT_MOUSE;
                    input.Anonymous.mi = MOUSEINPUT {
                        dx: 0,
                        dy: 0,
                        mouseData: if key.mouse_flags_up == MOUSEEVENTF_XUP {
                            if key.vk == VK_XBUTTON1 {
                                XBUTTON1 as u32
                            } else {
                                XBUTTON2 as u32
                            }
                        } else {
                            0
                        },
                        dwFlags: key.mouse_flags_up,
                        time: 0,
                        dwExtraInfo: 0,
                    };
                    SendInput(1, &input, std::mem::size_of::<INPUT>() as i32);
                } else {
                    let mut input = std::mem::zeroed::<INPUT>();
                    input.r#type = INPUT_KEYBOARD;
                    if input_mode == 0 && key.scan_code > 0 {
                        input.Anonymous.ki = KEYBDINPUT {
                            wVk: 0,
                            wScan: key.scan_code,
                            dwFlags: KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP | if key.is_extended { KEYEVENTF_EXTENDEDKEY } else { 0 },
                            time: 0,
                            dwExtraInfo: 0,
                        };
                    } else {
                        let mut flags = KEYEVENTF_KEYUP;
                        if key.is_extended {
                            flags |= KEYEVENTF_EXTENDEDKEY;
                        }
                        input.Anonymous.ki = KEYBDINPUT {
                            wVk: key.vk,
                            wScan: key.scan_code,
                            dwFlags: flags,
                            time: 0,
                            dwExtraInfo: 0,
                        };
                    }
                    SendInput(1, &input, std::mem::size_of::<INPUT>() as i32);
                }
            }
        }
    }

    pub fn is_vk_down(vk: u32) -> bool {
        unsafe {
            let state = GetAsyncKeyState(vk as i32);
            (state as u16 & 0x8000) != 0
        }
    }
}
