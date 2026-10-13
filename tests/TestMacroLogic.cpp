#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <filesystem>
#include <cmath>

#include "../src/Config.h"
#include "../src/InputManager.h"
#include "../src/MacroEngine.h"

// Simple testing framework macros
static int g_testsPassed = 0;
static int g_testsFailed = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cerr << "  [FAIL] " << msg << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; \
            g_testsFailed++; \
            return; \
        } \
    } while(0)

#define TEST_RUN(testFunc) \
    do { \
        std::cout << "[RUNNING] " << #testFunc << "..." << std::endl; \
        int failedBefore = g_testsFailed; \
        testFunc(); \
        if (g_testsFailed == failedBefore) { \
            std::cout << "  [PASS] " << #testFunc << std::endl; \
            g_testsPassed++; \
        } \
    } while(0)

// Test 1: User's exact combo sequence parsing
void TestUserDefaultSequenceParsing() {
    std::string seq = "2, 2, z, z, 2, 2, x, x, 3, 3";
    std::vector<MacroKeyStep> steps;
    std::string err;

    bool ok = InputManager::ParseSequence(seq, steps, err);
    TEST_ASSERT(ok, "Failed to parse default sequence: " + err);
    TEST_ASSERT(steps.size() == 10, "Expected 10 steps, got " + std::to_string(steps.size()));

    // Verify each exact key
    TEST_ASSERT(steps[0].label == "2" && steps[0].vk == '2', "Step 0 must be '2'");
    TEST_ASSERT(steps[1].label == "2" && steps[1].vk == '2', "Step 1 must be '2'");
    TEST_ASSERT(steps[2].label == "Z" && steps[2].vk == 'Z', "Step 2 must be 'Z'");
    TEST_ASSERT(steps[3].label == "Z" && steps[3].vk == 'Z', "Step 3 must be 'Z'");
    TEST_ASSERT(steps[4].label == "2" && steps[4].vk == '2', "Step 4 must be '2'");
    TEST_ASSERT(steps[5].label == "2" && steps[5].vk == '2', "Step 5 must be '2'");
    TEST_ASSERT(steps[6].label == "X" && steps[6].vk == 'X', "Step 6 must be 'X'");
    TEST_ASSERT(steps[7].label == "X" && steps[7].vk == 'X', "Step 7 must be 'X'");
    TEST_ASSERT(steps[8].label == "3" && steps[8].vk == '3', "Step 8 must be '3'");
    TEST_ASSERT(steps[9].label == "3" && steps[9].vk == '3', "Step 9 must be '3'");

    // Verify hardware scan codes are non-zero
    for (size_t i = 0; i < steps.size(); ++i) {
        TEST_ASSERT(steps[i].scanCode > 0, "Step " + std::to_string(i) + " scanCode must be non-zero");
    }
}

// Test 2: Delimiter variations (spaces, arrows, semicolons, pipes)
void TestDelimiterVariations() {
    std::vector<MacroKeyStep> steps;
    std::string err;

    // Space separated
    TEST_ASSERT(InputManager::ParseSequence("2 2 z z 2 2 x x 3 3", steps, err), "Space separated failed");
    TEST_ASSERT(steps.size() == 10, "Expected 10 steps for space separated");

    // Semicolon separated
    TEST_ASSERT(InputManager::ParseSequence("2;2;z;z;2;2;x;x;3;3", steps, err), "Semicolon separated failed");
    TEST_ASSERT(steps.size() == 10, "Expected 10 steps for semicolon separated");

    // Arrow syntax "2 -> 2 -> z -> z"
    TEST_ASSERT(InputManager::ParseSequence("2 -> 2 -> z -> z -> 2 -> 2 -> x -> x -> 3 -> 3", steps, err), "Arrow separated failed");
    TEST_ASSERT(steps.size() == 10, "Expected 10 steps for arrow separated");

    // Pipe separated
    TEST_ASSERT(InputManager::ParseSequence("2|2|z|z|2|2|x|x|3|3", steps, err), "Pipe separated failed");
    TEST_ASSERT(steps.size() == 10, "Expected 10 steps for pipe separated");
}

// Test 3: Named and special keys
void TestNamedKeys() {
    std::vector<MacroKeyStep> steps;
    std::string err;

    std::string seq = "SPACE LSHIFT LCTRL TAB ESC F1 F12";
    TEST_ASSERT(InputManager::ParseSequence(seq, steps, err), "Named keys parsing failed: " + err);
    TEST_ASSERT(steps.size() == 7, "Expected 7 named keys");

    TEST_ASSERT(steps[0].vk == VK_SPACE, "Expected VK_SPACE");
    TEST_ASSERT(steps[1].vk == VK_LSHIFT, "Expected VK_LSHIFT");
    TEST_ASSERT(steps[2].vk == VK_LCONTROL, "Expected VK_LCONTROL");
    TEST_ASSERT(steps[3].vk == VK_TAB, "Expected VK_TAB");
    TEST_ASSERT(steps[4].vk == VK_ESCAPE, "Expected VK_ESCAPE");
    TEST_ASSERT(steps[5].vk == VK_F1, "Expected VK_F1");
    TEST_ASSERT(steps[6].vk == VK_F12, "Expected VK_F12");
}

// Test 4: Extended keys and mouse button token parsing
void TestExtendedAndMouseInputParsing() {
    std::vector<MacroKeyStep> steps;
    std::string err;

    std::string seq = "LMB RMB MMB M4 M5 UP DOWN LEFT RIGHT NUM1 NUMPAD5 [ ]";
    TEST_ASSERT(InputManager::ParseSequence(seq, steps, err), "Extended keys parsing failed: " + err);
    TEST_ASSERT(steps.size() == 13, "Expected 13 steps");

    // Mouse
    TEST_ASSERT(steps[0].isMouse && steps[0].vk == VK_LBUTTON, "Expected LMB");
    TEST_ASSERT(steps[1].isMouse && steps[1].vk == VK_RBUTTON, "Expected RMB");
    TEST_ASSERT(steps[2].isMouse && steps[2].vk == VK_MBUTTON, "Expected MMB");
    TEST_ASSERT(steps[3].isMouse && steps[3].vk == VK_XBUTTON1, "Expected M4 / XBUTTON1");
    TEST_ASSERT(steps[4].isMouse && steps[4].vk == VK_XBUTTON2, "Expected M5 / XBUTTON2");

    // Arrows
    TEST_ASSERT(steps[5].isExtended && steps[5].vk == VK_UP, "Expected UP");
    TEST_ASSERT(steps[6].isExtended && steps[6].vk == VK_DOWN, "Expected DOWN");
    TEST_ASSERT(steps[7].isExtended && steps[7].vk == VK_LEFT, "Expected LEFT");
    TEST_ASSERT(steps[8].isExtended && steps[8].vk == VK_RIGHT, "Expected RIGHT");

    // Numpad
    TEST_ASSERT(steps[9].vk == VK_NUMPAD1, "Expected NUM1");
    TEST_ASSERT(steps[10].vk == VK_NUMPAD5, "Expected NUMPAD5");

    // Symbols via VkKeyScanA
    TEST_ASSERT(steps[11].label == "[", "Expected [");
    TEST_ASSERT(steps[12].label == "]", "Expected ]");
}

// Test 5: Invalid and edge-case inputs
void TestInvalidInputs() {
    std::vector<MacroKeyStep> steps;
    std::string err;

    // Empty string
    TEST_ASSERT(!InputManager::ParseSequence("", steps, err), "Empty string should fail");
    TEST_ASSERT(!err.empty(), "Empty string should return error message");

    // Whitespace only
    TEST_ASSERT(!InputManager::ParseSequence("   \t  \r\n  ", steps, err), "Whitespace string should fail");

    // Invalid token
    TEST_ASSERT(!InputManager::ParseSequence("2, 2, INVALID_UNKNOWN_KEY, 3", steps, err), "Unknown key should fail");
    TEST_ASSERT(err.find("INVALID_UNKNOWN_KEY") != std::string::npos, "Error should mention unrecognized token");
}

// Test 6: Hardware ScanCode mapping accuracy for Deadlock / Source 2
void TestScanCodeMapping() {
    MacroKeyStep step;

    // '2' scan code is 0x03 on standard IBM PC keyboards
    TEST_ASSERT(InputManager::ParseKeyToken("2", step), "Failed to parse '2'");
    TEST_ASSERT(step.scanCode == 0x03, "Scan code for '2' should be 0x03, got " + std::to_string(step.scanCode));

    // '3' scan code is 0x04
    TEST_ASSERT(InputManager::ParseKeyToken("3", step), "Failed to parse '3'");
    TEST_ASSERT(step.scanCode == 0x04, "Scan code for '3' should be 0x04, got " + std::to_string(step.scanCode));

    // 'Z' scan code is 0x2C
    TEST_ASSERT(InputManager::ParseKeyToken("z", step), "Failed to parse 'z'");
    TEST_ASSERT(step.scanCode == 0x2C, "Scan code for 'z' should be 0x2C, got " + std::to_string(step.scanCode));

    // 'X' scan code is 0x2D
    TEST_ASSERT(InputManager::ParseKeyToken("x", step), "Failed to parse 'x'");
    TEST_ASSERT(step.scanCode == 0x2D, "Scan code for 'x' should be 0x2D, got " + std::to_string(step.scanCode));
}

// Test 7: Configuration persistence (Save and Load INI)
void TestConfigPersistence() {
    std::string testPath = "test_settings_temp.ini";

    MacroSettings s1;
    s1.sequenceString = "2, 2, z, z, 2, 2, x, x, 3, 3";
    s1.triggerKeyVk = VK_XBUTTON1;
    s1.toggleHotkeyVk = VK_F6;
    s1.pressDelayMs = 500;
    s1.keyHoldDurationMs = 45;
    s1.doubleTapDelayMs = 70;
    s1.useFastDoubleTap = true;
    s1.jitterMs = 30;
    s1.loopDelayMs = 0;
    s1.inputMode = 0;
    s1.soundBeepOnToggle = false;
    s1.macroEnabled = true;
    s1.autoDoubleTap = true;

    TEST_ASSERT(s1.SaveToFile(testPath), "Failed to save test config");

    MacroSettings s2;
    TEST_ASSERT(s2.LoadFromFile(testPath), "Failed to load test config");

    TEST_ASSERT(s2.sequenceString == s1.sequenceString, "sequenceString mismatch");
    TEST_ASSERT(s2.triggerKeyVk == s1.triggerKeyVk, "triggerKeyVk mismatch");
    TEST_ASSERT(s2.toggleHotkeyVk == s1.toggleHotkeyVk, "toggleHotkeyVk mismatch");
    TEST_ASSERT(s2.pressDelayMs == 500, "pressDelayMs mismatch");
    TEST_ASSERT(s2.keyHoldDurationMs == 45, "keyHoldDurationMs mismatch");
    TEST_ASSERT(s2.doubleTapDelayMs == 70, "doubleTapDelayMs mismatch");
    TEST_ASSERT(s2.useFastDoubleTap == true, "useFastDoubleTap mismatch");
    TEST_ASSERT(s2.jitterMs == 30, "jitterMs mismatch");
    TEST_ASSERT(s2.loopDelayMs == 0, "loopDelayMs mismatch");
    TEST_ASSERT(s2.inputMode == 0, "inputMode mismatch");
    TEST_ASSERT(s2.soundBeepOnToggle == false, "soundBeepOnToggle mismatch");
    TEST_ASSERT(s2.macroEnabled == true, "macroEnabled mismatch");
    TEST_ASSERT(s2.autoDoubleTap == true, "autoDoubleTap mismatch");

    // Clean up temporary file
    std::filesystem::remove(testPath);
}

// Test 8: Clamping and boundary validation
void TestConfigBoundsClamping() {
    MacroSettings s;
    s.pressDelayMs = -100;
    s.keyHoldDurationMs = 2;
    s.jitterMs = 500;
    s.loopDelayMs = -50;
    s.inputMode = 99;
    s.sequenceString = "";

    s.ValidateAndClamp();

    TEST_ASSERT(s.pressDelayMs >= 20, "pressDelayMs should be clamped to minimum 20");
    TEST_ASSERT(s.keyHoldDurationMs >= 5, "keyHoldDurationMs should be clamped to minimum 5");
    TEST_ASSERT(s.jitterMs <= 200, "jitterMs should be clamped to maximum 200");
    TEST_ASSERT(s.loopDelayMs >= 0, "loopDelayMs should be clamped to non-negative");
    TEST_ASSERT(s.inputMode == 0, "invalid inputMode should reset to 0");
    TEST_ASSERT(!s.sequenceString.empty(), "empty sequence should reset to default sequence");
}

// Test 9: Real Abort Latency Verification (< 25ms mid-sequence interrupt)
void TestImmediateAbortLatency() {
    MacroSettings settings;
    settings.sequenceString = "2, 2, z, z, 2, 2, x, x, 3, 3";
    settings.pressDelayMs = 500;
    settings.macroEnabled = true;
    // Set trigger to an unpressed key (e.g. VK_F24) so it tests immediate abort
    settings.triggerKeyVk = VK_F24;

    MacroEngine engine;
    engine.Initialize(settings);

    // Measure time taken by InterruptibleSleep when trigger is unpressed
    auto start = std::chrono::steady_clock::now();
    bool sleepCompleted = engine.InterruptibleSleepForTest(500);
    auto end = std::chrono::steady_clock::now();
    int elapsedMs = (int)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Must return false immediately without waiting the full 500ms
    TEST_ASSERT(!sleepCompleted, "InterruptibleSleep must return false when trigger is not held");
    TEST_ASSERT(elapsedMs < 25, "Abort latency must be under 25ms, got " + std::to_string(elapsedMs) + "ms");

    // Test abort when macroEnabled is flipped
    engine.SetMacroEnabled(false);
    TEST_ASSERT(!engine.IsMacroEnabled(), "Macro must be disabled");

    engine.Shutdown();
    TEST_ASSERT(InputManager::GetActiveTrackedKeysCount() == 0, "All tracked simulated keys must be released");
}

// Test 10: Key Tracking and Safety Panic Release
void TestTrackedKeysSafetyRelease() {
    MacroKeyStep step;
    step.label = "2";
    step.vk = '2';
    step.scanCode = 0x03;

    // Simulate sending key down
    InputManager::SendKeyDown(step, 0);
    TEST_ASSERT(InputManager::GetActiveTrackedKeysCount() == 1, "Tracked keys count should be 1 after SendKeyDown");

    // Panic release
    InputManager::ReleaseAllSimulatedKeys(0);
    TEST_ASSERT(InputManager::GetActiveTrackedKeysCount() == 0, "Tracked keys count should be 0 after ReleaseAllSimulatedKeys");
}

// Test 11: Shorthand input without delimiters (User request: "123zc" -> 1,1,2,2,3,3,z,z,c,c)
void TestShorthandNoDelimiters() {
    std::vector<MacroKeyStep> steps;
    std::string err;

    bool ok = InputManager::ParseSequence("123zc", steps, err, true);
    TEST_ASSERT(ok, "Failed to parse shorthand '123zc': " + err);
    TEST_ASSERT(steps.size() == 10, "Expected 10 steps from '123zc', got " + std::to_string(steps.size()));

    TEST_ASSERT(steps[0].label == "1" && steps[0].vk == '1', "Step 0 must be '1'");
    TEST_ASSERT(steps[1].label == "1" && steps[1].vk == '1', "Step 1 must be '1'");
    TEST_ASSERT(steps[2].label == "2" && steps[2].vk == '2', "Step 2 must be '2'");
    TEST_ASSERT(steps[3].label == "2" && steps[3].vk == '2', "Step 3 must be '2'");
    TEST_ASSERT(steps[4].label == "3" && steps[4].vk == '3', "Step 4 must be '3'");
    TEST_ASSERT(steps[5].label == "3" && steps[5].vk == '3', "Step 5 must be '3'");
    TEST_ASSERT(steps[6].label == "Z" && steps[6].vk == 'Z', "Step 6 must be 'Z'");
    TEST_ASSERT(steps[7].label == "Z" && steps[7].vk == 'Z', "Step 7 must be 'Z'");
    TEST_ASSERT(steps[8].label == "C" && steps[8].vk == 'C', "Step 8 must be 'C'");
    TEST_ASSERT(steps[9].label == "C" && steps[9].vk == 'C', "Step 9 must be 'C'");
}

// Test 12: Shorthand with delimiters (e.g. "1,2,3,z,c") auto-doubled
void TestShorthandWithDelimiters() {
    std::vector<MacroKeyStep> steps;
    std::string err;

    bool ok = InputManager::ParseSequence("1,2,3,z,c", steps, err, true);
    TEST_ASSERT(ok, "Failed to parse '1,2,3,z,c': " + err);
    TEST_ASSERT(steps.size() == 10, "Expected 10 steps, got " + std::to_string(steps.size()));

    TEST_ASSERT(steps[0].vk == '1' && steps[1].vk == '1', "Expected doubled 1");
    TEST_ASSERT(steps[2].vk == '2' && steps[3].vk == '2', "Expected doubled 2");
    TEST_ASSERT(steps[4].vk == '3' && steps[5].vk == '3', "Expected doubled 3");
    TEST_ASSERT(steps[6].vk == 'Z' && steps[7].vk == 'Z', "Expected doubled Z");
    TEST_ASSERT(steps[8].vk == 'C' && steps[9].vk == 'C', "Expected doubled C");
}

// Test 13: Already doubled keys must NOT quadruple-expand
void TestAlreadyDoubledNoQuadruple() {
    std::vector<MacroKeyStep> steps;
    std::string err;

    bool ok = InputManager::ParseSequence("1, 1, 2, 2, 3, 3, z, z, c, c", steps, err, true);
    TEST_ASSERT(ok, "Failed to parse already doubled sequence: " + err);
    TEST_ASSERT(steps.size() == 10, "Expected 10 steps (not quadruple expanded 20), got " + std::to_string(steps.size()));

    // Also test without spaces: "112233zzcc"
    std::vector<MacroKeyStep> stepsConcatenated;
    ok = InputManager::ParseSequence("112233zzcc", stepsConcatenated, err, true);
    TEST_ASSERT(ok, "Failed to parse '112233zzcc': " + err);
    TEST_ASSERT(stepsConcatenated.size() == 10, "Expected 10 steps from '112233zzcc', got " + std::to_string(stepsConcatenated.size()));
}

// Test 14: Auto Double-Tap disabled returns single sequential presses
void TestAutoDoubleTapDisabled() {
    std::vector<MacroKeyStep> steps;
    std::string err;

    bool ok = InputManager::ParseSequence("123zc", steps, err, false);
    TEST_ASSERT(ok, "Failed to parse '123zc' with autoDoubleTap disabled: " + err);
    TEST_ASSERT(steps.size() == 5, "Expected 5 steps when autoDoubleTap is false, got " + std::to_string(steps.size()));

    TEST_ASSERT(steps[0].vk == '1', "Step 0 must be '1'");
    TEST_ASSERT(steps[1].vk == '2', "Step 1 must be '2'");
    TEST_ASSERT(steps[2].vk == '3', "Step 2 must be '3'");
    TEST_ASSERT(steps[3].vk == 'Z', "Step 3 must be 'Z'");
    TEST_ASSERT(steps[4].vk == 'C', "Step 4 must be 'C'");
}

// Test 15: Mixed single and already doubled keys
void TestMixedShorthandAndDoubled() {
    std::vector<MacroKeyStep> steps;
    std::string err;

    // '1' is single (should double), '2, 2' is doubled (should stay pair), '3' is single (should double)
    bool ok = InputManager::ParseSequence("1, 2, 2, 3", steps, err, true);
    TEST_ASSERT(ok, "Failed to parse mixed sequence: " + err);
    TEST_ASSERT(steps.size() == 6, "Expected 6 steps, got " + std::to_string(steps.size()));

    TEST_ASSERT(steps[0].vk == '1' && steps[1].vk == '1', "Expected doubled 1");
    TEST_ASSERT(steps[2].vk == '2' && steps[3].vk == '2', "Expected preserved pair 2");
    TEST_ASSERT(steps[4].vk == '3' && steps[5].vk == '3', "Expected doubled 3");
}

// Test 16: Compound shorthand with named keys
void TestShorthandWithNamedKeys() {
    std::vector<MacroKeyStep> steps;
    std::string err;

    bool ok = InputManager::ParseSequence("123 SPACE zc", steps, err, true);
    TEST_ASSERT(ok, "Failed to parse '123 SPACE zc': " + err);
    TEST_ASSERT(steps.size() == 12, "Expected 12 steps, got " + std::to_string(steps.size()));

    TEST_ASSERT(steps[0].vk == '1' && steps[1].vk == '1', "Expected 1, 1");
    TEST_ASSERT(steps[2].vk == '2' && steps[3].vk == '2', "Expected 2, 2");
    TEST_ASSERT(steps[4].vk == '3' && steps[5].vk == '3', "Expected 3, 3");
    TEST_ASSERT(steps[6].vk == VK_SPACE && steps[7].vk == VK_SPACE, "Expected SPACE, SPACE");
    TEST_ASSERT(steps[8].vk == 'Z' && steps[9].vk == 'Z', "Expected Z, Z");
    TEST_ASSERT(steps[10].vk == 'C' && steps[11].vk == 'C', "Expected C, C");
}

// Test 17: Function key with trailing characters (e.g. "F1Z" must decompose into F1 and Z, never swallow Z)
void TestFunctionKeyTrailingChars() {
    MacroKeyStep step;
    // F1Z is NOT a valid function key token by itself
    TEST_ASSERT(!InputManager::ParseKeyToken("F1Z", step), "ParseKeyToken('F1Z') should return false");

    // In sequence shorthand, "F1Z" should cleanly decompose into F1 and Z
    std::vector<MacroKeyStep> steps;
    std::string err;
    bool ok = InputManager::ParseSequence("F1Z", steps, err, false);
    TEST_ASSERT(ok, "ParseSequence('F1Z') should succeed: " + err);
    TEST_ASSERT(steps.size() == 2, "Expected 2 steps from 'F1Z', got " + std::to_string(steps.size()));
    TEST_ASSERT(steps[0].vk == VK_F1, "Step 0 must be VK_F1");
    TEST_ASSERT(steps[1].vk == 'Z', "Step 1 must be 'Z'");
}

// Test 18: Shorthand with printable symbols and brackets (e.g. "123[c")
void TestShorthandWithSymbols() {
    std::vector<MacroKeyStep> steps;
    std::string err;

    bool ok = InputManager::ParseSequence("123[c", steps, err, true);
    TEST_ASSERT(ok, "ParseSequence('123[c') should succeed: " + err);
    TEST_ASSERT(steps.size() == 10, "Expected 10 steps from '123[c', got " + std::to_string(steps.size()));
    TEST_ASSERT(steps[0].vk == '1' && steps[1].vk == '1', "Expected doubled 1");
    TEST_ASSERT(steps[2].vk == '2' && steps[3].vk == '2', "Expected doubled 2");
    TEST_ASSERT(steps[4].vk == '3' && steps[5].vk == '3', "Expected doubled 3");
    TEST_ASSERT(steps[6].label == "[" && steps[7].label == "[", "Expected doubled [");
    TEST_ASSERT(steps[8].vk == 'C' && steps[9].vk == 'C', "Expected doubled C");
}

int main() {
    std::cout << "============================================================" << std::endl;
    std::cout << " EMIL COMBAT SYSTEM · MACRO TEST SUITE" << std::endl;
    std::cout << "============================================================" << std::endl;

    TEST_RUN(TestUserDefaultSequenceParsing);
    TEST_RUN(TestDelimiterVariations);
    TEST_RUN(TestNamedKeys);
    TEST_RUN(TestExtendedAndMouseInputParsing);
    TEST_RUN(TestInvalidInputs);
    TEST_RUN(TestScanCodeMapping);
    TEST_RUN(TestConfigPersistence);
    TEST_RUN(TestConfigBoundsClamping);
    TEST_RUN(TestImmediateAbortLatency);
    TEST_RUN(TestTrackedKeysSafetyRelease);
    TEST_RUN(TestShorthandNoDelimiters);
    TEST_RUN(TestShorthandWithDelimiters);
    TEST_RUN(TestAlreadyDoubledNoQuadruple);
    TEST_RUN(TestAutoDoubleTapDisabled);
    TEST_RUN(TestMixedShorthandAndDoubled);
    TEST_RUN(TestShorthandWithNamedKeys);
    TEST_RUN(TestFunctionKeyTrailingChars);
    TEST_RUN(TestShorthandWithSymbols);

    std::cout << "============================================================" << std::endl;
    std::cout << " TEST SUMMARY: " << g_testsPassed << " PASSED, "
              << g_testsFailed << " FAILED" << std::endl;
    std::cout << "============================================================" << std::endl;

    return (g_testsFailed == 0) ? 0 : 1;
}
