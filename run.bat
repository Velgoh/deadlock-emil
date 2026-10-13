@echo off
cd /d "%~dp0"
if not exist "DeadlockMacro.exe" (
    echo [INFO] DeadlockMacro.exe not found. Running build.bat first...
    call build.bat
)
start "" "%~dp0DeadlockMacro.exe"
