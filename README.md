# Deadlock Emil Macro

A standalone, low-latency C++ Windows macro utility designed for ability rotations in Valve's *Deadlock* (Source 2).

Features an Atlus-inspired Dark Purple interface (Persona / Metaphor styling) rendered in DirectX 11 via Dear ImGui, with shorthand sequence expansion, sub-5ms trigger abort latency, and DirectInput hardware scancode injection.

*Glory to mankind.*

---

## How It Works

1. **Input Parser & Shorthand Expansion**:
   - Supports natural shorthand notation. Entering `123zc` automatically expands to `1, 1, 2, 2, 3, 3, z, z, c, c` when **Auto Double-Tap** is active.
   - Preserves already-doubled key pairs: inputs like `1, 1, 2, 2, 3, 3, z, z, c, c` will not be quadruple-expanded.
   - Delimiters (commas, spaces, semicolons, arrows `->`, pipes `|`) are supported interchangeably.
   - Auto Double-Tap can be toggled on/off in settings and persists in `settings.ini`.

2. **Trigger-Held Execution & Immediate Abort**:
   - Fires sequentially while the configured trigger key (default: `Mouse Back / M4`) is held down.
   - Releasing the trigger immediately aborts execution and releases any pressed keys in `< 5ms`.

3. **Humanized Timings & Anti-Detection**:
   - Inter-press delay: defaults to 500ms between key presses.
   - Key hold duration: 35ms actuation time.
   - Jitter: configurable randomized timing variance ($\pm 25\text{ ms}$).
   - Optional fast double-tap acceleration mode.

4. **DirectInput Hardware ScanCodes**:
   - Dispatches inputs via Windows `SendInput` using `KEYEVENTF_SCANCODE`, ensuring reliable registration in Source 2 without requiring virtual key message processing.

5. **Atlus Dark Purple Interface**:
   - Deep midnight obsidian background (`#0c0916`), elevated obsidian-violet cards (`#19122d`), electric violet borders (`#6b3db0`), and signature Atlus neon magenta highlights (`#e52df0`).
   - Razor-sharp angular geometry, slanted sequence step breadcrumbs, and clean telemetry badges.
   - Free of cluttered text and marketing fluff.

---

## Default Controls & Configuration

| Parameter | Default | Description |
|---|---|---|
| **Trigger Key** | `Mouse Back (XBUTTON1 / M4)` | Hold down to execute combo sequence |
| **Master Toggle** | `F6` | Global hotkey to engage / suspend the macro |
| **Sequence** | `2, 2, z, z, 2, 2, x, x, 3, 3` | Sequence string (or shorthand like `123zc`) |
| **Auto Double-Tap** | `Enabled` | Automatically doubles single keys in shorthand |
| **Inter-Press Delay** | `500 ms` | Cadence between sequential keypresses |
| **Key Hold Duration** | `35 ms` | Physical hold duration per simulated key |
| **Timing Jitter** | `± 25 ms` | Humanized random variance applied to timings |
| **Input Driver** | `Hardware ScanCode` | DirectInput scancode injection for Source 2 |

Configuration is automatically saved to and loaded from `settings.ini`.

---

## Building from Source

Requires Visual Studio 2022 Build Tools (or Community / Professional edition):

```cmd
build.bat
```

The build script:
1. Compiles and executes `TestMacroLogic.exe` (16 unit tests covering shorthand parsing, delimiter variants, named keys, auto double-tap logic, hardware scan codes, abort latency, and configuration persistence).
2. Compiles `DeadlockMacro.exe` with `/O2 /MT /std:c++17` linked against DirectX 11.

---

## Unit Test Suite

Run unit tests directly:

```cmd
TestMacroLogic.exe
```

Test coverage includes:
- User default combo parsing (`2, 2, z, z, 2, 2, x, x, 3, 3`)
- Delimiter variations (spaces, semicolons, arrows, pipes)
- Named and mouse keys (`SPACE`, `LSHIFT`, `LMB`, `M4`, `M5`, `F1`–`F24`)
- Shorthand token parsing (`123zc` -> 10 steps)
- Already-doubled sequence protection (no quadruple expansion)
- Auto Double-Tap toggle state verification
- Hardware scancode mapping for Deadlock
- Config persistence and boundary clamping
- Abort latency verification (< 25ms SLA)
- Key release safety panic handler
