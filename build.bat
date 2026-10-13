@echo off
setlocal enabledelayedexpansion

echo ==============================================================================
echo  // YORHA BUILD PROTOCOL // DEADLOCK MACRO COMPILATION
echo ==============================================================================

:: Check if cl is already in PATH
where cl >nul 2>nul
if %ERRORLEVEL% equ 0 (
    echo [OK] MSVC compiler already initialized in environment.
    goto COMPILE
)

:: Locate Visual Studio 2022 vcvars64.bat
set "VCVARS="
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" (
    set "VCVARS=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
    set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat" (
    set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat" (
    set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
)

if not defined VCVARS (
    echo [ERROR] Could not find vcvars64.bat. Please run from VS x64 Native Tools Command Prompt.
    pause
    exit /b 1
)

echo [INFO] Initializing x64 build environment: "!VCVARS!"
call "!VCVARS!"
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Failed to initialize Visual Studio environment.
    pause
    exit /b 1
)

:COMPILE
cd /d "%~dp0"

echo.
echo [PHASE 1/2] Compiling and Executing Unit Tests...
cl /nologo /O2 /MT /EHsc /std:c++17 /Fe:TestMacroLogic.exe tests\TestMacroLogic.cpp src\Config.cpp src\InputManager.cpp src\MacroEngine.cpp user32.lib
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Test compilation failed.
    pause
    exit /b 1
)

TestMacroLogic.exe
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Tests reported failures. Aborting release build.
    pause
    exit /b 1
)

echo.
echo [PHASE 2/2] Compiling Standalone GUI Executable (DeadlockMacro.exe)...
cl /nologo /O2 /MT /EHsc /std:c++17 /I imgui /I imgui\backends /I src ^
   src\main.cpp src\Config.cpp src\InputManager.cpp src\MacroEngine.cpp src\AtlusTheme.cpp ^
   imgui\imgui.cpp imgui\imgui_draw.cpp imgui\imgui_tables.cpp imgui\imgui_widgets.cpp ^
   imgui\backends\imgui_impl_win32.cpp imgui\backends\imgui_impl_dx11.cpp ^
   d3d11.lib dxgi.lib dwmapi.lib user32.lib gdi32.lib shell32.lib ^
   /Fe:DeadlockMacro.exe /link /SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup

if %ERRORLEVEL% neq 0 (
    echo [ERROR] Release build failed.
    pause
    exit /b 1
)

:: Clean up temporary object files
del /q *.obj >nul 2>nul

echo.
echo ==============================================================================
echo  // BUILD SUCCESSFUL // DeadlockMacro.exe READY FOR COMBAT
echo ==============================================================================
echo Output file: %~dp0DeadlockMacro.exe
echo.
