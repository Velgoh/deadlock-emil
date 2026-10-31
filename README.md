# Deadlock Macro (v2.0.0)

A high-performance, minimalist desktop macro utility built natively in Rust using `eframe` / `egui`. Designed specifically for low-latency combat and ability combo sequences in Deadlock.

![UI Preview](assets/zero.gif)

## Features

- **Native Rust & egui/eframe Architecture**: Ultra-lightweight, zero bloated web runtimes, instantaneous startup.
- **Minimalist Dark Aesthetic**: Clean, modern dark UI matching precision framepacer diagnostics (#141414 base, #242424 rounded toolbar buttons).
- **DirectInput Hardware Scancode Emulation**: Native Win32 `SendInput` hardware scancode dispatch ensuring 100% reliable input registration in Source 2.
- **Sub-5ms Abort Latency**: Multi-threaded interruptible sleep engine with continuous trigger polling for instantaneous mid-sequence cancellation upon trigger release.
- **Custom Vector Glyphs**: High-fidelity, scalable vector icons (Settings Drawer, Mode Toggle, Pin / Always-On-Top, Trigger Indicator, Arm Toggle).
- **Smooth Anti-Aliased Corner Masking**: Embedded companion GIF (`zero.gif`) with mathematical corner clipping ensuring zero corner overflow.
- **Auto Double-Tap Expansion**: Automatically doubles single abilities into rapid double-taps (e.g. shorthand `123zc` or `23zx`) while smartly preserving existing pairs without quadruple expansion.
- **Humanized Timing & Jitter Variance**: Configurable press delays, key hold durations, loop delays, and uniform timing jitter variance.
- **Full Configuration Persistence**: Automatically saves and loads configuration from `settings.ini`.

## Key Controls

| Control | Default | Description |
|---|---|---|
| **Trigger Key** | `Mouse 4` (`XBUTTON1`) | Hold to execute the active macro sequence. Releasing aborts in <5ms. |
| **Global Toggle Hotkey** | `F6` | Toggle the macro between Armed and Standby. |
| **Settings Drawer** | `≡` (Hamburger) | Expand settings drawer for timing adjustments, trigger keys, and modes. |
| **Mode Toggle** | `⊞` (Grid) | Toggle between Hardware ScanCode (Source 2) and VirtualKey mode. |
| **Pin Toggle** | `📌` (Pin) | Toggle always-on-top window layering. |
| **Double-Tap Toggle** | `2x Tap: ON/OFF` | Toggle auto double-tap shorthand expansion. |

## Sequence Syntax

The sequence parser accepts both shorthand and delimited formats:

- **Shorthand**: `123zc`, `23zx`, `112233zzcc`
- **Delimited**: `1, 1, 2, 2, z, z`, `2 -> 2 -> z -> z`, `1 2 3 z c`, `2;2;z;z`
- **Named Keys**: `SPACE`, `LSHIFT`, `LCTRL`, `TAB`, `ESC`, `F1` - `F12`, `UP`, `DOWN`, `LEFT`, `RIGHT`
- **Mouse Buttons**: `LMB`, `RMB`, `MMB`, `M4`, `M5`

## Building from Source

### Prerequisites

- [Rust Toolchain](https://rustup.rs/) (1.75+ or later)
- Windows 10 / 11

### Compile

```bash
cargo build --release
```

The compiled standalone executable will be located at `target/release/DeadlockMacro.exe`.

### Run Tests

```bash
cargo test
```

## Support

For issues, bug reports, or feature suggestions, please open an issue on GitHub.

Glory to mankind.

## License

MIT License.

