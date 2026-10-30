use crate::config::{
    get_default_settings_path, get_key_name_by_vk, MacroSettings, HOTKEY_OPTIONS,
    TRIGGER_KEY_OPTIONS,
};
use crate::engine::MacroEngine;
use crate::gif_player::{GifPlayer, EMBEDDED_ZERO_GIF};
use egui::{
    pos2, vec2, Align, Align2, Color32, FontId, Layout, Painter, Rect, Response, Rounding, Sense,
    Stroke, Ui, Vec2, ViewportCommand,
};
use std::sync::Arc;

pub struct DeadlockApp {
    pub engine: Arc<MacroEngine>,
    pub gif_player: Option<GifPlayer>,
    pub settings_open: bool,
    pub sequence_input: String,
    pub always_on_top: bool,
    pub status_toast: Option<(String, std::time::Instant)>,
}

impl DeadlockApp {
    pub fn new(cc: &eframe::CreationContext<'_>) -> Self {
        // Configure dark theme matching framepacer
        let mut visuals = egui::Visuals::dark();
        visuals.panel_fill = Color32::from_rgb(0x16, 0x16, 0x16);
        visuals.window_fill = Color32::from_rgb(0x16, 0x16, 0x16);
        visuals.faint_bg_color = Color32::from_rgb(0x1E, 0x1E, 0x1E);
        visuals.widgets.noninteractive.bg_fill = Color32::from_rgb(0x1E, 0x1E, 0x1E);
        visuals.widgets.noninteractive.bg_stroke = Stroke::new(1.0_f32, Color32::from_rgb(0x2A, 0x2A, 0x2A));
        visuals.widgets.noninteractive.rounding = Rounding::same(6.0);
        visuals.widgets.inactive.bg_fill = Color32::from_rgb(0x24, 0x24, 0x24);
        visuals.widgets.inactive.bg_stroke = Stroke::new(1.0_f32, Color32::from_rgb(0x32, 0x32, 0x32));
        visuals.widgets.inactive.rounding = Rounding::same(6.0);
        visuals.widgets.hovered.bg_fill = Color32::from_rgb(0x34, 0x34, 0x34);
        visuals.widgets.hovered.bg_stroke = Stroke::new(1.0_f32, Color32::from_rgb(0x44, 0x44, 0x44));
        visuals.widgets.hovered.rounding = Rounding::same(6.0);
        visuals.widgets.active.bg_fill = Color32::from_rgb(0x40, 0x40, 0x40);
        visuals.widgets.active.bg_stroke = Stroke::new(1.0_f32, Color32::from_rgb(0x55, 0x55, 0x55));
        visuals.widgets.active.rounding = Rounding::same(6.0);
        cc.egui_ctx.set_visuals(visuals);

        // Load settings
        let mut settings = MacroSettings::default();
        let path = get_default_settings_path();
        settings.load_from_file(path);

        let always_on_top = settings.always_on_top;
        let sequence_input = settings.sequence_string.clone();

        // Load Zero GIF with corner rounding mask (scaled radius 14.0px -> ~8.8px on screen)
        let gif_player = match GifPlayer::load_from_memory(EMBEDDED_ZERO_GIF, 14.0) {
            Ok(p) => Some(p),
            Err(e) => {
                eprintln!("Failed to load embedded Zero GIF: {}", e);
                None
            }
        };

        let engine = Arc::new(MacroEngine::new(settings));

        Self {
            engine,
            gif_player,
            settings_open: false,
            sequence_input,
            always_on_top,
            status_toast: None,
        }
    }
}

impl eframe::App for DeadlockApp {
    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        // Update GIF animation
        if let Some(player) = &mut self.gif_player {
            player.update(ctx);
        }

        // Apply always-on-top viewport command if needed
        let engine_settings = self.engine.get_settings();
        if self.always_on_top != engine_settings.always_on_top {
            self.always_on_top = engine_settings.always_on_top;
            ctx.send_viewport_cmd(ViewportCommand::WindowLevel(if self.always_on_top {
                egui::WindowLevel::AlwaysOnTop
            } else {
                egui::WindowLevel::Normal
            }));
        }

        egui::CentralPanel::default()
            .frame(egui::Frame::none().fill(Color32::from_rgb(0x14, 0x14, 0x14)).inner_margin(8.0))
            .show(ctx, |ui| {
                ui.spacing_mut().item_spacing = vec2(6.0, 6.0);

                // 1. Top Toolbar Row (Matching framepacer_zVnc1Pde3a.png)
                self.render_toolbar(ui, ctx);

                // 2. Expandable Settings Drawer
                if self.settings_open {
                    self.render_settings_drawer(ui);
                }

                // 3. Sequence Input Row
                self.render_sequence_row(ui);

                // 4. Telemetry / Status Readout
                self.render_telemetry(ui);

                // 5. Bottom Panel: Zero GIF
                self.render_gif_panel(ui, ctx);
            });
    }
}

impl DeadlockApp {
    fn render_toolbar(&mut self, ui: &mut Ui, ctx: &egui::Context) {
        ui.horizontal(|ui| {
            ui.spacing_mut().item_spacing = vec2(6.0, 0.0);

            // Button 1: Hamburger Button (Settings Drawer Toggle)
            let hamburger_btn = dark_square_button(ui, vec2(32.0, 28.0), self.settings_open);
            draw_hamburger_icon(ui.painter(), hamburger_btn.rect, Color32::from_rgb(0xD0, 0xD0, 0xD0));
            if hamburger_btn.clicked() {
                self.settings_open = !self.settings_open;
            }

            // Button 2: Mode / Grid Button (Input Mode Toggle: ScanCode <-> VirtualKey)
            let is_scancode = self.engine.get_settings().input_mode == 0;
            let grid_btn = dark_square_button(ui, vec2(32.0, 28.0), is_scancode);
            draw_grid_icon(ui.painter(), grid_btn.rect, Color32::from_rgb(0xD0, 0xD0, 0xD0));
            if grid_btn.clicked() {
                let mut s = self.engine.get_settings();
                s.input_mode = if s.input_mode == 0 { 1 } else { 0 };
                let mode_str = if s.input_mode == 0 { "Hardware ScanCode" } else { "Virtual Key" };
                self.engine.set_settings(s.clone());
                s.save_to_file(get_default_settings_path());
                self.show_toast(format!("Input Mode: {}", mode_str));
            }

            // Button 3: Pin Button (Always-on-top Toggle)
            let pin_btn = dark_square_button(ui, vec2(32.0, 28.0), self.always_on_top);
            draw_pin_icon(ui.painter(), pin_btn.rect, Color32::from_rgb(0xD0, 0xD0, 0xD0), self.always_on_top);
            if pin_btn.clicked() {
                self.always_on_top = !self.always_on_top;
                let mut s = self.engine.get_settings();
                s.always_on_top = self.always_on_top;
                self.engine.set_settings(s.clone());
                s.save_to_file(get_default_settings_path());
                ctx.send_viewport_cmd(ViewportCommand::WindowLevel(if self.always_on_top {
                    egui::WindowLevel::AlwaysOnTop
                } else {
                    egui::WindowLevel::Normal
                }));
                self.show_toast(format!("Always-on-top: {}", if self.always_on_top { "ON" } else { "OFF" }));
            }

            // Vertical Divider
            let (div_rect, _) = ui.allocate_exact_size(vec2(8.0, 28.0), Sense::hover());
            ui.painter().line_segment(
                [pos2(div_rect.center().x, div_rect.top() + 4.0), pos2(div_rect.center().x, div_rect.bottom() - 4.0)],
                Stroke::new(1.0_f32, Color32::from_rgb(0x33, 0x33, 0x33)),
            );

            // Hotkey / Trigger indicator box
            let s = self.engine.get_settings();
            let trigger_name = get_key_name_by_vk(s.trigger_key_vk, TRIGGER_KEY_OPTIONS, "M4");
            let indicator_label = if trigger_name.contains("Mouse 4") {
                "M4".to_string()
            } else if trigger_name.contains("Mouse 5") {
                "M5".to_string()
            } else if trigger_name.contains("Mouse 1") {
                "M1".to_string()
            } else if trigger_name.contains("Mouse 2") {
                "M2".to_string()
            } else if trigger_name.contains("Mouse 3") {
                "M3".to_string()
            } else {
                trigger_name.split_whitespace().next().unwrap_or("0").to_string()
            };

            let (ind_rect, _) = ui.allocate_exact_size(vec2(44.0, 28.0), Sense::hover());
            ui.painter().rect(
                ind_rect,
                Rounding::same(6.0),
                Color32::from_rgb(0x1E, 0x1E, 0x1E),
                Stroke::new(1.0_f32, Color32::from_rgb(0x2C, 0x2C, 0x2C)),
            );
            ui.painter().text(
                ind_rect.center(),
                Align2::CENTER_CENTER,
                indicator_label,
                FontId::proportional(12.0),
                Color32::from_rgb(0xE0, 0xE0, 0xE0),
            );

            // Right-aligned Arm / Toggle button ("Apply" / "Armed")
            ui.with_layout(Layout::right_to_left(Align::Center), |ui| {
                let is_armed = self.engine.is_macro_enabled();
                let label = if is_armed { "Armed" } else { "Apply" };
                let bg_color = if is_armed {
                    Color32::from_rgb(0x24, 0x38, 0x24)
                } else {
                    Color32::from_rgb(0x24, 0x24, 0x24)
                };
                let text_color = if is_armed {
                    Color32::from_rgb(0x66, 0xBB, 0x6A)
                } else {
                    Color32::from_rgb(0xD0, 0xD0, 0xD0)
                };

                let btn = custom_rounded_button(ui, vec2(68.0, 28.0), bg_color, text_color, label);
                if btn.clicked() {
                    self.engine.toggle_macro_enabled();
                }
            });
        });
    }

    fn render_settings_drawer(&mut self, ui: &mut Ui) {
        egui::Frame::none()
            .fill(Color32::from_rgb(0x1A, 0x1A, 0x1A))
            .stroke(Stroke::new(1.0_f32, Color32::from_rgb(0x2E, 0x2E, 0x2E)))
            .rounding(Rounding::same(6.0))
            .inner_margin(8.0)
            .show(ui, |ui| {
                let mut s = self.engine.get_settings();
                let mut changed = false;

                ui.label(egui::RichText::new("Macro Settings").size(11.0).color(Color32::from_rgb(0x88, 0x88, 0x88)));

                // Trigger Key & Toggle Hotkey selection
                ui.horizontal(|ui| {
                    ui.label(egui::RichText::new("Trigger:").size(11.0).color(Color32::from_rgb(0xBB, 0xBB, 0xBB)));
                    let cur_trig_name = get_key_name_by_vk(s.trigger_key_vk, TRIGGER_KEY_OPTIONS, "Unknown");
                    egui::ComboBox::from_id_salt("trigger_combo")
                        .selected_text(cur_trig_name)
                        .show_ui(ui, |ui| {
                            for opt in TRIGGER_KEY_OPTIONS {
                                if ui.selectable_value(&mut s.trigger_key_vk, opt.vk, opt.name).clicked() {
                                    changed = true;
                                }
                            }
                        });

                    ui.label(egui::RichText::new("Hotkey:").size(11.0).color(Color32::from_rgb(0xBB, 0xBB, 0xBB)));
                    let cur_hk_name = get_key_name_by_vk(s.toggle_hotkey_vk, HOTKEY_OPTIONS, "F6");
                    egui::ComboBox::from_id_salt("hotkey_combo")
                        .selected_text(cur_hk_name)
                        .show_ui(ui, |ui| {
                            for opt in HOTKEY_OPTIONS {
                                if ui.selectable_value(&mut s.toggle_hotkey_vk, opt.vk, opt.name).clicked() {
                                    changed = true;
                                }
                            }
                        });
                });

                // Press Delay & Hold Duration
                ui.horizontal(|ui| {
                    ui.label(egui::RichText::new("Press Delay:").size(11.0));
                    if ui.add(egui::Slider::new(&mut s.press_delay_ms, 20..=1000).suffix("ms")).changed() {
                        changed = true;
                    }
                    ui.label(egui::RichText::new("Hold:").size(11.0));
                    if ui.add(egui::Slider::new(&mut s.key_hold_duration_ms, 5..=200).suffix("ms")).changed() {
                        changed = true;
                    }
                });

                // Timing Jitter & Double Tap Delay
                ui.horizontal(|ui| {
                    ui.label(egui::RichText::new("Jitter:").size(11.0));
                    if ui.add(egui::Slider::new(&mut s.jitter_ms, 0..=100).suffix("ms")).changed() {
                        changed = true;
                    }
                    ui.label(egui::RichText::new("2x Delay:").size(11.0));
                    if ui.add(egui::Slider::new(&mut s.double_tap_delay_ms, 20..=500).suffix("ms")).changed() {
                        changed = true;
                    }
                });

                // Toggles
                ui.horizontal(|ui| {
                    if ui.checkbox(&mut s.sound_beep_on_toggle, "Sound Beep").changed() {
                        changed = true;
                    }
                    if ui.checkbox(&mut s.use_fast_double_tap, "Fast 2x Tap").changed() {
                        changed = true;
                    }
                });

                // Save & Reset Buttons
                ui.horizontal(|ui| {
                    if ui.button("Save Settings").clicked() {
                        s.validate_and_clamp();
                        self.engine.set_settings(s.clone());
                        s.save_to_file(get_default_settings_path());
                        self.show_toast("Settings saved to settings.ini".to_string());
                    }
                    if ui.button("Reset Defaults").clicked() {
                        s.reset_to_defaults();
                        self.engine.set_settings(s.clone());
                        s.save_to_file(get_default_settings_path());
                        self.sequence_input = s.sequence_string.clone();
                        self.show_toast("Reset to defaults".to_string());
                    }
                });

                if changed {
                    s.validate_and_clamp();
                    self.engine.set_settings(s.clone());
                    s.save_to_file(get_default_settings_path());
                }
            });
    }

    fn render_sequence_row(&mut self, ui: &mut Ui) {
        ui.horizontal(|ui| {
            ui.spacing_mut().item_spacing = vec2(6.0, 0.0);

            let mut current_seq = self.sequence_input.clone();
            let response = ui.add(
                egui::TextEdit::singleline(&mut current_seq)
                    .hint_text("Sequence (e.g. 123zc or 2,2,z,z)")
                    .desired_width(ui.available_width() - 90.0)
                    .font(FontId::monospace(12.0)),
            );

            if response.changed() {
                self.sequence_input = current_seq.clone();
                let mut s = self.engine.get_settings();
                s.sequence_string = current_seq;
                self.engine.set_settings(s.clone());
                s.save_to_file(get_default_settings_path());
            }

            // Double-Tap toggle button
            let mut s = self.engine.get_settings();
            let is_dt = s.auto_double_tap;
            let dt_label = if is_dt { "2x Tap: ON" } else { "2x Tap: OFF" };
            let bg_color = if is_dt {
                Color32::from_rgb(0x28, 0x38, 0x48)
            } else {
                Color32::from_rgb(0x24, 0x24, 0x24)
            };
            let text_color = if is_dt {
                Color32::from_rgb(0x64, 0xB5, 0xF6)
            } else {
                Color32::from_rgb(0x99, 0x99, 0x99)
            };

            let btn = custom_rounded_button(ui, vec2(84.0, 24.0), bg_color, text_color, dt_label);
            if btn.clicked() {
                s.auto_double_tap = !s.auto_double_tap;
                self.engine.set_settings(s.clone());
                s.save_to_file(get_default_settings_path());
            }
        });

        // Sequence badge chips or error notice
        let err = self.engine.get_parse_error();
        if !err.is_empty() {
            ui.label(egui::RichText::new(&err).size(10.5).color(Color32::from_rgb(0xE5, 0x73, 0x73)));
        }
    }

    fn render_telemetry(&mut self, ui: &mut Ui) {
        let is_armed = self.engine.is_macro_enabled();
        let s = self.engine.get_settings();
        let trigger_name = get_key_name_by_vk(s.trigger_key_vk, TRIGGER_KEY_OPTIONS, "M4");
        let cycles = self.engine.get_cycle_count();
        let keys = self.engine.get_keys_pressed_count();

        // Row 1: Status & Trigger
        ui.horizontal(|ui| {
            let status_text = if is_armed { "Status: Armed" } else { "Status: Standby" };
            let status_color = if is_armed {
                Color32::from_rgb(0x81, 0xC7, 0x84)
            } else {
                Color32::from_rgb(0xE0, 0xE0, 0xE0)
            };
            ui.label(egui::RichText::new(status_text).size(12.0).color(status_color));
            ui.label(egui::RichText::new("|").size(12.0).color(Color32::from_rgb(0x55, 0x55, 0x55)));
            ui.label(
                egui::RichText::new(format!("Trigger: {}", trigger_name))
                    .size(12.0)
                    .color(Color32::from_rgb(0x4F, 0xC3, 0xF7)),
            );

            if let Some((msg, inst)) = &self.status_toast {
                if inst.elapsed() < std::time::Duration::from_secs(3) {
                    ui.label(
                        egui::RichText::new(format!("· {}", msg))
                            .size(11.0)
                            .color(Color32::from_rgb(0xFF, 0xD5, 0x4F)),
                    );
                }
            }
        });

        // Row 2: Performance counters
        ui.label(
            egui::RichText::new(format!(
                "Cycles: {} | Keys: {} | Hold: {}ms | Delay: {}ms",
                cycles, keys, s.key_hold_duration_ms, s.press_delay_ms
            ))
            .size(11.0)
            .color(Color32::from_rgb(0x88, 0x88, 0x88)),
        );
    }

    fn render_gif_panel(&mut self, ui: &mut Ui, ctx: &egui::Context) {
        let avail_size = ui.available_size();
        let height = avail_size.y.max(120.0);

        egui::Frame::none()
            .fill(Color32::from_rgb(0x18, 0x18, 0x18))
            .stroke(Stroke::new(1.0_f32, Color32::from_rgb(0x28, 0x28, 0x28)))
            .rounding(Rounding::same(8.0))
            .inner_margin(2.0)
            .show(ui, |ui| {
                let rect = ui.available_rect_before_wrap();
                let panel_rect = Rect::from_min_size(rect.min, vec2(ui.available_width(), height));

                if let Some(player) = &mut self.gif_player {
                    let (pw, ph) = (player.width as f32, player.height as f32);
                    if let Some(tex) = player.texture(ctx) {
                        // Maintain aspect ratio centered within the container
                        let aspect = pw / ph;
                        let target_w = panel_rect.width();
                        let target_h = target_w / aspect;

                        let final_size = if target_h <= panel_rect.height() {
                            vec2(target_w, target_h)
                        } else {
                            vec2(panel_rect.height() * aspect, panel_rect.height())
                        };

                        let image_rect = Rect::from_center_size(panel_rect.center(), final_size);

                        // Draw the texture. The corner mask in GifPlayer guarantees corners never bleed!
                        ui.painter().image(
                            tex.id(),
                            image_rect,
                            Rect::from_min_max(pos2(0.0, 0.0), pos2(1.0, 1.0)),
                            Color32::WHITE,
                        );
                    }
                } else {
                    ui.painter().text(
                        panel_rect.center(),
                        Align2::CENTER_CENTER,
                        "Zero GIF",
                        FontId::proportional(12.0),
                        Color32::from_rgb(0x66, 0x66, 0x66),
                    );
                }

                ui.allocate_rect(panel_rect, Sense::hover());
            });
    }

    fn show_toast(&mut self, msg: String) {
        self.status_toast = Some((msg, std::time::Instant::now()));
    }
}

// ----------------------------------------------------------------------------
// Custom Vector UI Controls (Replacing old procedural / pixel icons)
// ----------------------------------------------------------------------------

fn dark_square_button(ui: &mut Ui, size: Vec2, active: bool) -> Response {
    let (rect, response) = ui.allocate_exact_size(size, Sense::click());
    let bg = if active {
        Color32::from_rgb(0x38, 0x38, 0x38)
    } else if response.is_pointer_button_down_on() {
        Color32::from_rgb(0x40, 0x40, 0x40)
    } else if response.hovered() {
        Color32::from_rgb(0x34, 0x34, 0x34)
    } else {
        Color32::from_rgb(0x24, 0x24, 0x24)
    };

    let stroke = Stroke::new(
        1.0_f32,
        if active {
            Color32::from_rgb(0x48, 0x48, 0x48)
        } else {
            Color32::from_rgb(0x30, 0x30, 0x30)
        },
    );

    ui.painter().rect(rect, Rounding::same(6.0), bg, stroke);
    response
}

fn custom_rounded_button(ui: &mut Ui, size: Vec2, bg: Color32, text_color: Color32, label: &str) -> Response {
    let (rect, response) = ui.allocate_exact_size(size, Sense::click());
    let final_bg = if response.is_pointer_button_down_on() {
        Color32::from_rgb(
            bg.r().saturating_add(25),
            bg.g().saturating_add(25),
            bg.b().saturating_add(25),
        )
    } else if response.hovered() {
        Color32::from_rgb(
            bg.r().saturating_add(15),
            bg.g().saturating_add(15),
            bg.b().saturating_add(15),
        )
    } else {
        bg
    };

    ui.painter().rect(rect, Rounding::same(6.0), final_bg, Stroke::new(1.0_f32, Color32::from_rgb(0x36, 0x36, 0x36)));
    ui.painter().text(
        rect.center(),
        Align2::CENTER_CENTER,
        label,
        FontId::proportional(12.0),
        text_color,
    );

    response
}

/// Clean 3-bar hamburger glyph
fn draw_hamburger_icon(painter: &Painter, rect: Rect, color: Color32) {
    let c = rect.center();
    let half_w = 7.0;
    let stroke = Stroke::new(1.8_f32, color);

    painter.line_segment([pos2(c.x - half_w, c.y - 4.5), pos2(c.x + half_w, c.y - 4.5)], stroke);
    painter.line_segment([pos2(c.x - half_w, c.y), pos2(c.x + half_w, c.y)], stroke);
    painter.line_segment([pos2(c.x - half_w, c.y + 4.5), pos2(c.x + half_w, c.y + 4.5)], stroke);
}

/// Clean 2x2 grid glyph (mode button)
fn draw_grid_icon(painter: &Painter, rect: Rect, color: Color32) {
    let c = rect.center();
    let gap = 1.5;
    let size = 4.5;

    let tl = Rect::from_min_max(pos2(c.x - size - gap, c.y - size - gap), pos2(c.x - gap, c.y - gap));
    let tr = Rect::from_min_max(pos2(c.x + gap, c.y - size - gap), pos2(c.x + size + gap, c.y - gap));
    let bl = Rect::from_min_max(pos2(c.x - size - gap, c.y + gap), pos2(c.x - gap, c.y + size + gap));
    let br = Rect::from_min_max(pos2(c.x + gap, c.y + gap), pos2(c.x + size + gap, c.y + size + gap));

    painter.rect_filled(tl, Rounding::same(1.0), color);
    painter.rect_filled(tr, Rounding::same(1.0), color);
    painter.rect_filled(bl, Rounding::same(1.0), color);
    painter.rect_filled(br, Rounding::same(1.0), color);
}

/// Sleek pushpin vector icon
fn draw_pin_icon(painter: &Painter, rect: Rect, color: Color32, is_pinned: bool) {
    let c = rect.center();
    let active_color = if is_pinned {
        Color32::from_rgb(0x64, 0xB5, 0xF6)
    } else {
        color
    };

    let stroke = Stroke::new(1.6_f32, active_color);

    // Pin head (top angled bar)
    let p_head_l = pos2(c.x - 3.5, c.y - 5.5);
    let p_head_r = pos2(c.x + 3.5, c.y - 5.5);
    painter.line_segment([p_head_l, p_head_r], stroke);

    // Pin body
    let p_body_top = pos2(c.x, c.y - 5.5);
    let p_body_mid = pos2(c.x, c.y + 1.0);
    painter.line_segment([p_body_top, p_body_mid], stroke);

    // Pin base bar
    let p_base_l = pos2(c.x - 4.5, c.y + 1.0);
    let p_base_r = pos2(c.x + 4.5, c.y + 1.0);
    painter.line_segment([p_base_l, p_base_r], stroke);

    // Pin needle
    let p_needle_tip = pos2(c.x, c.y + 7.0);
    painter.line_segment([p_body_mid, p_needle_tip], Stroke::new(1.4_f32, active_color));
}
