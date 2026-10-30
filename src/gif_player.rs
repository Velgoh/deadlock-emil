use egui::{Color32, ColorImage, Context, TextureHandle, TextureOptions, Vec2};
use image::codecs::gif::GifDecoder;
use image::{AnimationDecoder, RgbaImage};
use std::io::Cursor;
use std::time::{Duration, Instant};

pub const EMBEDDED_ZERO_GIF: &[u8] = include_bytes!("../assets/zero.gif");

pub struct FrameData {
    pub image: ColorImage,
    pub delay: Duration,
}

pub struct GifPlayer {
    frames: Vec<FrameData>,
    textures: Vec<Option<TextureHandle>>,
    current_index: usize,
    last_frame_instant: Instant,
    pub width: usize,
    pub height: usize,
}

impl GifPlayer {
    pub fn load_from_memory(bytes: &[u8], corner_radius: f32) -> Result<Self, String> {
        let cursor = Cursor::new(bytes);
        let decoder = GifDecoder::new(cursor).map_err(|e| format!("GIF decode error: {}", e))?;
        let raw_frames = decoder
            .into_frames()
            .collect_frames()
            .map_err(|e| format!("GIF frame collection error: {}", e))?;

        if raw_frames.is_empty() {
            return Err("GIF has no frames".to_string());
        }

        let first = &raw_frames[0];
        let width = first.buffer().width() as usize;
        let height = first.buffer().height() as usize;

        let mut frames = Vec::with_capacity(raw_frames.len());

        for raw_frame in raw_frames {
            let (numer, denom) = raw_frame.delay().numer_denom_ms();
            let delay_ms = if denom == 0 || numer == 0 {
                100
            } else {
                (numer / denom).max(10)
            };

            let mut img_buf: RgbaImage = raw_frame.into_buffer();

            // Anti-aliased corner rounding mask
            if corner_radius > 0.0 {
                apply_rounded_corners(&mut img_buf, corner_radius);
            }

            let pixels: Vec<Color32> = img_buf
                .pixels()
                .map(|p| Color32::from_rgba_premultiplied(p[0], p[1], p[2], p[3]))
                .collect();

            let color_image = ColorImage {
                size: [width, height],
                pixels,
            };

            frames.push(FrameData {
                image: color_image,
                delay: Duration::from_millis(delay_ms as u64),
            });
        }

        let textures = vec![None; frames.len()];

        Ok(Self {
            frames,
            textures,
            current_index: 0,
            last_frame_instant: Instant::now(),
            width,
            height,
        })
    }

    pub fn update(&mut self, ctx: &Context) {
        if self.frames.is_empty() {
            return;
        }

        let current_delay = self.frames[self.current_index].delay;
        let elapsed = self.last_frame_instant.elapsed();

        if elapsed >= current_delay {
            self.current_index = (self.current_index + 1) % self.frames.len();
            self.last_frame_instant = Instant::now();
            ctx.request_repaint_after(self.frames[self.current_index].delay);
        } else {
            ctx.request_repaint_after(current_delay.saturating_sub(elapsed));
        }
    }

    pub fn texture(&mut self, ctx: &Context) -> Option<&TextureHandle> {
        if self.frames.is_empty() {
            return None;
        }

        let idx = self.current_index;
        if self.textures[idx].is_none() {
            let name = format!("zero_gif_frame_{}", idx);
            let handle = ctx.load_texture(&name, self.frames[idx].image.clone(), TextureOptions::LINEAR);
            self.textures[idx] = Some(handle);
        }

        self.textures[idx].as_ref()
    }

    pub fn size(&self) -> Vec2 {
        Vec2::new(self.width as f32, self.height as f32)
    }
}

/// Applies silky smooth anti-aliased corner rounding mask to the RGBA buffer.
/// Corners outside `radius` will have alpha = 0. Edge pixels are anti-aliased.
fn apply_rounded_corners(img: &mut RgbaImage, radius: f32) {
    let w = img.width() as f32;
    let h = img.height() as f32;
    let r = radius.min(w * 0.5).min(h * 0.5);

    if r <= 0.0 {
        return;
    }

    for y in 0..img.height() {
        let py = y as f32 + 0.5;
        for x in 0..img.width() {
            let px = x as f32 + 0.5;

            // Determine if inside any corner quadrant
            let (cx, cy) = if px < r && py < r {
                (r, r) // Top-Left
            } else if px > w - r && py < r {
                (w - r, r) // Top-Right
            } else if px < r && py > h - r {
                (r, h - r) // Bottom-Left
            } else if px > w - r && py > h - r {
                (w - r, h - r) // Bottom-Right
            } else {
                continue;
            };

            let dist = ((px - cx).powi(2) + (py - cy).powi(2)).sqrt();
            if dist > r - 1.0 {
                let pixel = img.get_pixel_mut(x, y);
                if dist >= r {
                    pixel[3] = 0;
                } else {
                    let factor = (r - dist).clamp(0.0, 1.0);
                    pixel[3] = (pixel[3] as f32 * factor).round() as u8;
                }
            }
        }
    }
}
