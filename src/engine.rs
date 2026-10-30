use crate::config::MacroSettings;
use crate::input::{InputManager, MacroKeyStep};
use chrono::Local;
use rand::Rng;
use std::collections::VecDeque;
use std::sync::atomic::{AtomicBool, AtomicI32, AtomicU64, Ordering};
use std::sync::{Arc, Mutex, RwLock};
use std::thread;
use std::time::{Duration, Instant};
#[link(name = "user32")]
extern "system" {
    fn MessageBeep(uType: u32) -> i32;
}

#[derive(Clone, Debug)]
pub struct LogEntry {
    pub timestamp: String,
    pub message: String,
    pub level: u8, // 0 = Info, 1 = Success/Active, 2 = Warning/Alert
}

pub struct MacroEngine {
    settings: Arc<RwLock<MacroSettings>>,
    running: Arc<AtomicBool>,
    macro_enabled: Arc<AtomicBool>,
    trigger_held: Arc<AtomicBool>,
    sequence_running: Arc<AtomicBool>,
    current_step_index: Arc<AtomicI32>,
    cycle_count: Arc<AtomicU64>,
    keys_pressed_count: Arc<AtomicU64>,
    parsed_steps: Arc<RwLock<Vec<MacroKeyStep>>>,
    parse_error: Arc<RwLock<String>>,
    logs: Arc<Mutex<VecDeque<LogEntry>>>,
    _worker_thread: Option<thread::JoinHandle<()>>,
}

impl MacroEngine {
    pub fn new(initial_settings: MacroSettings) -> Self {
        let settings = Arc::new(RwLock::new(initial_settings.clone()));
        let running = Arc::new(AtomicBool::new(true));
        let macro_enabled = Arc::new(AtomicBool::new(initial_settings.macro_enabled));
        let trigger_held = Arc::new(AtomicBool::new(false));
        let sequence_running = Arc::new(AtomicBool::new(false));
        let current_step_index = Arc::new(AtomicI32::new(-1));
        let cycle_count = Arc::new(AtomicU64::new(0));
        let keys_pressed_count = Arc::new(AtomicU64::new(0));
        let parsed_steps = Arc::new(RwLock::new(Vec::new()));
        let parse_error = Arc::new(RwLock::new(String::new()));
        let logs = Arc::new(Mutex::new(VecDeque::new()));

        // Parse initial sequence
        {
            let mut steps = Vec::new();
            let mut err = String::new();
            InputManager::parse_sequence(
                &initial_settings.sequence_string,
                &mut steps,
                &mut err,
                initial_settings.auto_double_tap,
            );
            *parsed_steps.write().unwrap() = steps;
            *parse_error.write().unwrap() = err;
        }

        // Add startup log
        {
            let mut q = logs.lock().unwrap();
            q.push_back(LogEntry {
                timestamp: Local::now().format("%H:%M:%S%.3f").to_string(),
                message: "Deadlock Macro Engine initialized [eframe/Rust v2.0.0]".to_string(),
                level: 1,
            });
        }

        let worker_settings = Arc::clone(&settings);
        let worker_running = Arc::clone(&running);
        let worker_macro_enabled = Arc::clone(&macro_enabled);
        let worker_trigger_held = Arc::clone(&trigger_held);
        let worker_sequence_running = Arc::clone(&sequence_running);
        let worker_current_step = Arc::clone(&current_step_index);
        let worker_cycle_count = Arc::clone(&cycle_count);
        let worker_keys_pressed = Arc::clone(&keys_pressed_count);
        let worker_parsed_steps = Arc::clone(&parsed_steps);
        let worker_logs = Arc::clone(&logs);

        let handle = thread::spawn(move || {
            let mut was_hotkey_down = false;
            let mut was_trigger_down = false;

            while worker_running.load(Ordering::Relaxed) {
                // 1. Check Hotkey Edge (F6 toggle)
                let (hotkey_vk, sound_beep) = {
                    let s = worker_settings.read().unwrap();
                    (s.toggle_hotkey_vk, s.sound_beep_on_toggle)
                };

                let hotkey_down = InputManager::is_vk_down(hotkey_vk);
                if hotkey_down && !was_hotkey_down {
                    let current = worker_macro_enabled.load(Ordering::Relaxed);
                    let new_state = !current;
                    worker_macro_enabled.store(new_state, Ordering::Relaxed);
                    if let Ok(mut s) = worker_settings.write() {
                        s.macro_enabled = new_state;
                    }

                    if sound_beep {
                        unsafe { MessageBeep(0xFFFFFFFF) };
                    }

                    let ts = Local::now().format("%H:%M:%S%.3f").to_string();
                    if new_state {
                        if let Ok(mut q) = worker_logs.lock() {
                            q.push_back(LogEntry {
                                timestamp: ts,
                                message: "SYSTEM STATUS: ENGAGED [ACTIVE]".to_string(),
                                level: 1,
                            });
                        }
                    } else {
                        let input_mode = worker_settings.read().map(|s| s.input_mode).unwrap_or(0);
                        InputManager::release_all_simulated_keys(input_mode);
                        worker_sequence_running.store(false, Ordering::Relaxed);
                        worker_current_step.store(-1, Ordering::Relaxed);

                        if let Ok(mut q) = worker_logs.lock() {
                            q.push_back(LogEntry {
                                timestamp: ts,
                                message: "SYSTEM STATUS: SUSPENDED [OFF]".to_string(),
                                level: 2,
                            });
                        }
                    }
                }
                was_hotkey_down = hotkey_down;

                // 2. Read execution parameters
                let (trigger_vk, press_delay, key_hold, double_tap_delay, fast_double, jitter, loop_delay, input_mode) = {
                    let s = worker_settings.read().unwrap();
                    (
                        s.trigger_key_vk,
                        s.press_delay_ms,
                        s.key_hold_duration_ms,
                        s.double_tap_delay_ms,
                        s.use_fast_double_tap,
                        s.jitter_ms,
                        s.loop_delay_ms,
                        s.input_mode,
                    )
                };

                let trigger_down = InputManager::is_vk_down(trigger_vk);
                worker_trigger_held.store(trigger_down, Ordering::Relaxed);

                if trigger_down && !was_trigger_down {
                    let ts = Local::now().format("%H:%M:%S%.3f").to_string();
                    let enabled = worker_macro_enabled.load(Ordering::Relaxed);
                    if let Ok(mut q) = worker_logs.lock() {
                        if enabled {
                            q.push_back(LogEntry {
                                timestamp: ts,
                                message: "TRIGGER ENGAGED · EXECUTING COMBAT SEQUENCE".to_string(),
                                level: 1,
                            });
                        } else {
                            q.push_back(LogEntry {
                                timestamp: ts,
                                message: "TRIGGER DETECTED · MACRO IS SUSPENDED [IGNORED]".to_string(),
                                level: 0,
                            });
                        }
                    }
                }
                was_trigger_down = trigger_down;

                let steps: Vec<MacroKeyStep> = worker_parsed_steps.read().unwrap().clone();
                let is_enabled = worker_macro_enabled.load(Ordering::Relaxed);

                if !is_enabled || !trigger_down || steps.is_empty() {
                    if worker_sequence_running.load(Ordering::Relaxed) {
                        InputManager::release_all_simulated_keys(input_mode);
                        worker_sequence_running.store(false, Ordering::Relaxed);
                        worker_current_step.store(-1, Ordering::Relaxed);
                    }
                    thread::sleep(Duration::from_millis(5));
                    continue;
                }

                // 3. Active sequence execution
                worker_sequence_running.store(true, Ordering::Relaxed);
                let mut aborted = false;

                for (idx, step) in steps.iter().enumerate() {
                    // Check abort before each step
                    if !worker_running.load(Ordering::Relaxed)
                        || !worker_macro_enabled.load(Ordering::Relaxed)
                        || !InputManager::is_vk_down(trigger_vk)
                    {
                        aborted = true;
                        break;
                    }

                    worker_current_step.store(idx as i32, Ordering::Relaxed);

                    // A. Press key down
                    InputManager::send_key_down(step, input_mode);

                    // B. Hold key with sub-5ms interruptible sleep
                    let hold_jitter = calculate_jitter(jitter);
                    let actual_hold = (key_hold as i32 + hold_jitter).max(10) as u32;

                    if !interruptible_sleep_slice(
                        actual_hold,
                        trigger_vk,
                        &worker_running,
                        &worker_macro_enabled,
                    ) {
                        aborted = true;
                        break;
                    }

                    // C. Release key
                    InputManager::send_key_up(step, input_mode);
                    worker_keys_pressed.fetch_add(1, Ordering::Relaxed);

                    // D. Delay between steps
                    let mut step_delay = press_delay;
                    if fast_double && idx + 1 < steps.len() && steps[idx].vk == steps[idx + 1].vk {
                        step_delay = double_tap_delay;
                    }

                    let delay_jitter = calculate_jitter(jitter);
                    let actual_delay = (step_delay as i32 + delay_jitter).max(10) as u32;

                    if !interruptible_sleep_slice(
                        actual_delay,
                        trigger_vk,
                        &worker_running,
                        &worker_macro_enabled,
                    ) {
                        aborted = true;
                        break;
                    }
                }

                if aborted {
                    InputManager::release_all_simulated_keys(input_mode);
                    worker_sequence_running.store(false, Ordering::Relaxed);
                    worker_current_step.store(-1, Ordering::Relaxed);

                    let ts = Local::now().format("%H:%M:%S%.3f").to_string();
                    if let Ok(mut q) = worker_logs.lock() {
                        q.push_back(LogEntry {
                            timestamp: ts,
                            message: "TRIGGER RELEASED · IMMEDIATE MID-SEQUENCE ABORT (<5ms)".to_string(),
                            level: 0,
                        });
                    }
                } else {
                    worker_cycle_count.fetch_add(1, Ordering::Relaxed);
                    worker_current_step.store(-1, Ordering::Relaxed);

                    if loop_delay > 0 && InputManager::is_vk_down(trigger_vk) {
                        let loop_jitter = calculate_jitter(jitter);
                        let actual_loop = (loop_delay as i32 + loop_jitter).max(0) as u32;
                        if actual_loop > 0 {
                            if !interruptible_sleep_slice(
                                actual_loop,
                                trigger_vk,
                                &worker_running,
                                &worker_macro_enabled,
                            ) {
                                InputManager::release_all_simulated_keys(input_mode);
                                worker_sequence_running.store(false, Ordering::Relaxed);
                            }
                        }
                    }
                }
            }

            // Cleanup when worker thread terminates
            InputManager::release_all_simulated_keys(0);
        });

        Self {
            settings,
            running,
            macro_enabled,
            trigger_held,
            sequence_running,
            current_step_index,
            cycle_count,
            keys_pressed_count,
            parsed_steps,
            parse_error,
            logs,
            _worker_thread: Some(handle),
        }
    }

    pub fn set_settings(&self, new_settings: MacroSettings) {
        let mut steps = Vec::new();
        let mut err = String::new();
        InputManager::parse_sequence(
            &new_settings.sequence_string,
            &mut steps,
            &mut err,
            new_settings.auto_double_tap,
        );

        *self.parsed_steps.write().unwrap() = steps;
        *self.parse_error.write().unwrap() = err;
        self.macro_enabled.store(new_settings.macro_enabled, Ordering::Relaxed);
        *self.settings.write().unwrap() = new_settings;
    }

    pub fn get_settings(&self) -> MacroSettings {
        self.settings.read().unwrap().clone()
    }

    pub fn is_macro_enabled(&self) -> bool {
        self.macro_enabled.load(Ordering::Relaxed)
    }

    pub fn set_macro_enabled(&self, enabled: bool) {
        let prev = self.macro_enabled.swap(enabled, Ordering::Relaxed);
        if prev != enabled {
            if let Ok(mut s) = self.settings.write() {
                s.macro_enabled = enabled;
            }
            let ts = Local::now().format("%H:%M:%S%.3f").to_string();
            if enabled {
                self.add_log(&format!("SYSTEM STATUS: ENGAGED [ACTIVE] at {}", ts), 1);
            } else {
                self.panic_release();
                self.add_log(&format!("SYSTEM STATUS: SUSPENDED [OFF] at {}", ts), 2);
            }
        }
    }

    pub fn toggle_macro_enabled(&self) {
        self.set_macro_enabled(!self.is_macro_enabled());
    }

    pub fn is_trigger_held(&self) -> bool {
        self.trigger_held.load(Ordering::Relaxed)
    }

    pub fn is_sequence_running(&self) -> bool {
        self.sequence_running.load(Ordering::Relaxed)
    }

    pub fn get_current_step_index(&self) -> i32 {
        self.current_step_index.load(Ordering::Relaxed)
    }

    pub fn get_total_steps(&self) -> usize {
        self.parsed_steps.read().unwrap().len()
    }

    pub fn get_cycle_count(&self) -> u64 {
        self.cycle_count.load(Ordering::Relaxed)
    }

    pub fn get_keys_pressed_count(&self) -> u64 {
        self.keys_pressed_count.load(Ordering::Relaxed)
    }

    pub fn get_parsed_steps(&self) -> Vec<MacroKeyStep> {
        self.parsed_steps.read().unwrap().clone()
    }

    pub fn get_parse_error(&self) -> String {
        self.parse_error.read().unwrap().clone()
    }

    pub fn get_recent_logs(&self, max_count: usize) -> Vec<LogEntry> {
        let q = self.logs.lock().unwrap();
        let start = if q.len() > max_count { q.len() - max_count } else { 0 };
        q.iter().skip(start).cloned().collect()
    }

    pub fn clear_logs(&self) {
        self.logs.lock().unwrap().clear();
    }

    pub fn add_log(&self, msg: &str, level: u8) {
        let mut q = self.logs.lock().unwrap();
        q.push_back(LogEntry {
            timestamp: Local::now().format("%H:%M:%S%.3f").to_string(),
            message: msg.to_string(),
            level,
        });
        if q.len() > 200 {
            q.pop_front();
        }
    }

    pub fn panic_release(&self) {
        let input_mode = self.settings.read().map(|s| s.input_mode).unwrap_or(0);
        InputManager::release_all_simulated_keys(input_mode);
        self.sequence_running.store(false, Ordering::Relaxed);
        self.current_step_index.store(-1, Ordering::Relaxed);
    }

    pub fn interruptible_sleep_for_test(&self, duration_ms: u32) -> bool {
        let trigger_vk = self.settings.read().map(|s| s.trigger_key_vk).unwrap_or(0x05);
        interruptible_sleep_slice(duration_ms, trigger_vk, &self.running, &self.macro_enabled)
    }

    pub fn shutdown(&self) {
        self.running.store(false, Ordering::Relaxed);
        self.panic_release();
    }
}

impl Drop for MacroEngine {
    fn drop(&mut self) {
        self.shutdown();
    }
}

fn calculate_jitter(max_jitter: u32) -> i32 {
    if max_jitter == 0 {
        return 0;
    }
    let mut rng = rand::thread_rng();
    let j = max_jitter as i32;
    rng.gen_range(-j..=j)
}

/// Precise sub-5ms interruptible sleep slice.
/// Returns false immediately if trigger is released or engine is disabled/stopped.
fn interruptible_sleep_slice(
    duration_ms: u32,
    trigger_vk: u32,
    running: &AtomicBool,
    macro_enabled: &AtomicBool,
) -> bool {
    if duration_ms == 0 {
        return true;
    }

    let start = Instant::now();
    let target = Duration::from_millis(duration_ms as u64);

    while start.elapsed() < target {
        if !running.load(Ordering::Relaxed) || !macro_enabled.load(Ordering::Relaxed) {
            return false;
        }

        if !InputManager::is_vk_down(trigger_vk) {
            return false;
        }

        let remaining = target.saturating_sub(start.elapsed());
        let slice = remaining.min(Duration::from_millis(2));
        thread::sleep(slice);
    }

    true
}
