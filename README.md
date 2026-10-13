# emil m[A]cro

A lightweight, low-latency Windows input macro utility written in native C++ (DirectX 11 / Dear ImGui).

Features a Dark Purple UI overhaul with generous typography, responsive visual step feedback, shorthand sequence parsing, and sub-5ms trigger abort latency.

---

## How It Works

1. **Input Parser & Shorthand Notation**:
   - Accepts both delimited sequences (`1, 1, 2, 2, 3, 3, z, z, c, c`) and shorthand strings (`123zc`).
   - When **Auto Double-Tap** is enabled, single shorthand characters are automatically expanded into confirmation double-taps (`123zc` becomes `1, 1, 2, 2, 3, 3, z, z, c, c`).
   - Already-doubled pairs are recognized and preserved without quadruple expansion.
   - Supported delimiters include commas, spaces, semicolons, arrows (`->`), and vertical pipes (`|`).

2. **Trigger-Held Execution & Immediate Abort**:
   - The sequence fires continuously as long as the configured trigger key (default: `Mouse Back / M4`) is held down.
   - Releasing the trigger key immediately aborts the active sequence in `< 5ms` and releases any held keys to prevent stuck inputs.

3. **Timing Engine & Randomization**:
   - Inter-press delay: Configurable delay between sequential keypresses (default: 500 ms).
   - Key hold duration: Actuation duration for each keypress (default: 35 ms).
   - Timing jitter: Configurable randomized variance (default: ±25 ms) applied to timings to prevent robotic cadences.
   - Fast double-tap mode: Allows setting a custom rapid delay for double-tap pairs.

4. **Hardware ScanCode Injection**:
   - Inputs are dispatched through the Windows `SendInput` API using `KEYEVENTF_SCANCODE` rather than virtual key codes, ensuring reliable registration in games that read DirectInput hardware scans directly.

5. **Interface**:
   - Dark purple color scheme (`#0a0714` background, `#140d24` cards, electric purple and neon magenta highlights, high-contrast text).
   - Skewed cards, tilted ransom-style badges, diamond sliders, and dynamic step visualization.
   - Dynamic window title alternating through NieR ending bracket variations (`emil m[A]cro`, `[E]mil macro`, etc.).

---

## Default Controls & Configuration

| Parameter | Default | Description |
|---|---|---|
| **Trigger Key** | `Mouse Back (XBUTTON1 / M4)` | Hold down to run the combo sequence |
| **Master Toggle** | `F6` | Global hotkey to engage or suspend the macro |
| **Sequence** | `2, 2, z, z, 2, 2, x, x, 3, 3` | Active key sequence string or shorthand |
| **Auto Double-Tap** | `Enabled` | Expands single shorthand keys to double-taps |
| **Inter-Press Delay** | `500 ms` | Cadence between sequential inputs |
| **Key Hold Duration** | `35 ms` | Physical hold duration per input |
| **Timing Jitter** | `± 25 ms` | Randomized variance applied to timings |
| **Input Driver** | `Hardware ScanCode` | DirectInput scancode injection |

Settings are automatically persisted to and reloaded from `settings.ini`.

---

## Building from Source

Requires Visual Studio 2022 (Community, Professional, or Build Tools):

```cmd
build.bat
```

The build script will:
1. Compile and run the unit test suite (`TestMacroLogic.exe`), verifying all 18 test cases.
2. Compile the standalone executable (`DeadlockMacro.exe`) with `/O2 /MT /std:c++17` linked against DirectX 11.

---

## Running Tests

Run the test suite directly:

```cmd
TestMacroLogic.exe
```

The test suite covers:
- Key sequence parsing with standard and custom delimiters
- Shorthand token expansion (`123zc` -> 10 steps)
- Protection against duplicate expansion for pre-doubled sequences
- Named keys and mouse buttons (`SPACE`, `LSHIFT`, `M4`, `M5`, `F1`–`F24`)
- DirectInput scan code mappings
- Configuration persistence and boundary clamping
- Abort latency SLA (< 25ms) and key release panic safety
- Trailing character handling on multi-character tokens

---

## Support

For issues, bug reports, or feature suggestions, please open an issue on GitHub.

Glory to mankind.
