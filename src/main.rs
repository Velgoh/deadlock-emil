#![windows_subsystem = "windows"]

use deadlock_macro::config::{get_default_settings_path, MacroSettings};
use deadlock_macro::ui::DeadlockApp;

fn main() -> eframe::Result<()> {
    let mut initial_settings = MacroSettings::default();
    let settings_path = get_default_settings_path();
    initial_settings.load_from_file(settings_path);

    let mut viewport = egui::ViewportBuilder::default()
        .with_inner_size([362.0, 260.0])
        .with_min_inner_size([320.0, 220.0])
        .with_title("Deadlock")
        .with_decorations(true)
        .with_resizable(true);

    if initial_settings.always_on_top {
        viewport = viewport.with_always_on_top();
    }

    let native_options = eframe::NativeOptions {
        viewport,
        ..Default::default()
    };

    eframe::run_native(
        "Deadlock",
        native_options,
        Box::new(|cc| Ok(Box::new(DeadlockApp::new(cc)))),
    )
}
