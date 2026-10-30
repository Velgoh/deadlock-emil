use std::fs;
use std::path::{Path, PathBuf};

#[derive(Clone, Debug, PartialEq)]
pub struct MacroSettings {
    pub sequence_string: String,
    pub trigger_key_vk: u32,
    pub toggle_hotkey_vk: u32,
    pub press_delay_ms: u32,
    pub key_hold_duration_ms: u32,
    pub double_tap_delay_ms: u32,
    pub use_fast_double_tap: bool,
    pub jitter_ms: u32,
    pub loop_delay_ms: u32,
    pub input_mode: u32, // 0 = Hardware ScanCode, 1 = VirtualKey
    pub sound_beep_on_toggle: bool,
    pub macro_enabled: bool,
    pub auto_double_tap: bool,
    pub always_on_top: bool,
}

impl Default for MacroSettings {
    fn default() -> Self {
        Self {
            sequence_string: "23zx".to_string(),
            trigger_key_vk: 0x05,    // VK_XBUTTON1 (Mouse 4)
            toggle_hotkey_vk: 0x75,  // VK_F6
            press_delay_ms: 100,
            key_hold_duration_ms: 35,
            double_tap_delay_ms: 100,
            use_fast_double_tap: false,
            jitter_ms: 25,
            loop_delay_ms: 0,
            input_mode: 0,
            sound_beep_on_toggle: false,
            macro_enabled: true,
            auto_double_tap: true,
            always_on_top: true,
        }
    }
}

impl MacroSettings {
    pub fn reset_to_defaults(&mut self) {
        *self = Self::default();
    }

    pub fn validate_and_clamp(&mut self) {
        self.press_delay_ms = self.press_delay_ms.clamp(20, 2000);
        self.key_hold_duration_ms = self.key_hold_duration_ms.clamp(5, 500);
        self.double_tap_delay_ms = self.double_tap_delay_ms.clamp(20, 1000);

        if self.jitter_ms > 200 {
            self.jitter_ms = 200;
        }

        if self.input_mode > 1 {
            self.input_mode = 0;
        }

        let trimmed = self.sequence_string.trim();
        if trimmed.is_empty() {
            self.sequence_string = "23zx".to_string();
        }
    }

    pub fn load_from_file<P: AsRef<Path>>(&mut self, path: P) -> bool {
        let content = match fs::read_to_string(path) {
            Ok(c) => c,
            Err(_) => return false,
        };

        for line in content.lines() {
            let line = line.trim();
            if line.starts_with(';') || line.starts_with('#') || line.starts_with('[') || line.is_empty() {
                continue;
            }

            if let Some((key, val)) = line.split_once('=') {
                let k = key.trim().to_lowercase();
                let v = val.trim();

                match k.as_str() {
                    "sequence" => self.sequence_string = v.to_string(),
                    "trigger_key_vk" => {
                        if let Ok(num) = v.parse::<u32>() {
                            self.trigger_key_vk = num;
                        }
                    }
                    "toggle_hotkey_vk" => {
                        if let Ok(num) = v.parse::<u32>() {
                            self.toggle_hotkey_vk = num;
                        }
                    }
                    "press_delay_ms" => {
                        if let Ok(num) = v.parse::<u32>() {
                            self.press_delay_ms = num;
                        }
                    }
                    "key_hold_ms" | "key_hold_duration_ms" => {
                        if let Ok(num) = v.parse::<u32>() {
                            self.key_hold_duration_ms = num;
                        }
                    }
                    "double_tap_delay_ms" => {
                        if let Ok(num) = v.parse::<u32>() {
                            self.double_tap_delay_ms = num;
                        }
                    }
                    "use_fast_double_tap" => {
                        self.use_fast_double_tap = v == "1" || v.eq_ignore_ascii_case("true");
                    }
                    "jitter_ms" => {
                        if let Ok(num) = v.parse::<u32>() {
                            self.jitter_ms = num;
                        }
                    }
                    "loop_delay_ms" => {
                        if let Ok(num) = v.parse::<u32>() {
                            self.loop_delay_ms = num;
                        }
                    }
                    "input_mode" => {
                        if let Ok(num) = v.parse::<u32>() {
                            self.input_mode = num;
                        }
                    }
                    "sound_beep" | "sound_beep_on_toggle" => {
                        self.sound_beep_on_toggle = v == "1" || v.eq_ignore_ascii_case("true");
                    }
                    "macro_enabled" => {
                        self.macro_enabled = v == "1" || v.eq_ignore_ascii_case("true");
                    }
                    "auto_double_tap" => {
                        self.auto_double_tap = v == "1" || v.eq_ignore_ascii_case("true");
                    }
                    "always_on_top" => {
                        self.always_on_top = v == "1" || v.eq_ignore_ascii_case("true");
                    }
                    _ => {}
                }
            }
        }

        self.validate_and_clamp();
        true
    }

    pub fn save_to_file<P: AsRef<Path>>(&self, path: P) -> bool {
        let content = format!(
            "; Deadlock Macro Configuration\n\
             ; Automatically generated settings file\n\n\
             [Settings]\n\
             sequence = {}\n\
             trigger_key_vk = {}\n\
             toggle_hotkey_vk = {}\n\
             press_delay_ms = {}\n\
             key_hold_ms = {}\n\
             double_tap_delay_ms = {}\n\
             use_fast_double_tap = {}\n\
             jitter_ms = {}\n\
             loop_delay_ms = {}\n\
             input_mode = {}\n\
             sound_beep = {}\n\
             macro_enabled = {}\n\
             auto_double_tap = {}\n\
             always_on_top = {}\n",
            self.sequence_string,
            self.trigger_key_vk,
            self.toggle_hotkey_vk,
            self.press_delay_ms,
            self.key_hold_duration_ms,
            self.double_tap_delay_ms,
            if self.use_fast_double_tap { 1 } else { 0 },
            self.jitter_ms,
            self.loop_delay_ms,
            self.input_mode,
            if self.sound_beep_on_toggle { 1 } else { 0 },
            if self.macro_enabled { 1 } else { 0 },
            if self.auto_double_tap { 1 } else { 0 },
            if self.always_on_top { 1 } else { 0 }
        );

        fs::write(path, content).is_ok()
    }
}

pub fn get_default_settings_path() -> PathBuf {
    if let Ok(exe) = std::env::current_exe() {
        if let Some(dir) = exe.parent() {
            let p = dir.join("settings.ini");
            if p.exists() {
                return p;
            }
        }
    }
    let p = PathBuf::from("settings.ini");
    if p.exists() {
        return p;
    }
    let core_p = PathBuf::from("core/settings.ini");
    if core_p.exists() {
        return core_p;
    }
    p
}

#[derive(Clone, Copy, Debug, PartialEq)]
pub struct KeyOption {
    pub vk: u32,
    pub name: &'static str,
}

pub const TRIGGER_KEY_OPTIONS: &[KeyOption] = &[
    KeyOption { vk: 0x05, name: "Mouse 4 / Back" },
    KeyOption { vk: 0x06, name: "Mouse 5 / Forward" },
    KeyOption { vk: 0x01, name: "Mouse 1 / LMB" },
    KeyOption { vk: 0x02, name: "Mouse 2 / RMB" },
    KeyOption { vk: 0x04, name: "Mouse 3 / MMB" },
    KeyOption { vk: 0x70, name: "F1" },
    KeyOption { vk: 0x71, name: "F2" },
    KeyOption { vk: 0x72, name: "F3" },
    KeyOption { vk: 0x73, name: "F4" },
    KeyOption { vk: 0x74, name: "F5" },
    KeyOption { vk: 0x75, name: "F6" },
    KeyOption { vk: 0x76, name: "F7" },
    KeyOption { vk: 0x77, name: "F8" },
    KeyOption { vk: 0x78, name: "F9" },
    KeyOption { vk: 0x79, name: "F10" },
    KeyOption { vk: 0x7A, name: "F11" },
    KeyOption { vk: 0x7B, name: "F12" },
    KeyOption { vk: 0x20, name: "SPACE" },
    KeyOption { vk: 0xA0, name: "LSHIFT" },
    KeyOption { vk: 0xA2, name: "LCTRL" },
    KeyOption { vk: 0xA4, name: "LALT" },
    KeyOption { vk: 0x51, name: "Q" },
    KeyOption { vk: 0x45, name: "E" },
    KeyOption { vk: 0x52, name: "R" },
    KeyOption { vk: 0x46, name: "F" },
    KeyOption { vk: 0x56, name: "V" },
    KeyOption { vk: 0x43, name: "C" },
    KeyOption { vk: 0x58, name: "X" },
    KeyOption { vk: 0x5A, name: "Z" },
    KeyOption { vk: 0x31, name: "1" },
    KeyOption { vk: 0x32, name: "2" },
    KeyOption { vk: 0x33, name: "3" },
    KeyOption { vk: 0x34, name: "4" },
];

pub const HOTKEY_OPTIONS: &[KeyOption] = &[
    KeyOption { vk: 0x75, name: "F6 [Default]" },
    KeyOption { vk: 0x70, name: "F1" },
    KeyOption { vk: 0x71, name: "F2" },
    KeyOption { vk: 0x72, name: "F3" },
    KeyOption { vk: 0x73, name: "F4" },
    KeyOption { vk: 0x74, name: "F5" },
    KeyOption { vk: 0x76, name: "F7" },
    KeyOption { vk: 0x77, name: "F8" },
    KeyOption { vk: 0x78, name: "F9" },
    KeyOption { vk: 0x79, name: "F10" },
    KeyOption { vk: 0x7A, name: "F11" },
    KeyOption { vk: 0x7B, name: "F12" },
    KeyOption { vk: 0x24, name: "HOME" },
    KeyOption { vk: 0x23, name: "END" },
    KeyOption { vk: 0x2D, name: "INSERT" },
    KeyOption { vk: 0x2E, name: "DELETE" },
];

pub fn get_key_name_by_vk(vk: u32, options: &[KeyOption], fallback: &str) -> String {
    for opt in options {
        if opt.vk == vk {
            return opt.name.to_string();
        }
    }
    format!("{} (0x{:X})", fallback, vk)
}
