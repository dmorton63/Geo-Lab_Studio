@echo off
echo ==================================
echo Landscape Designer - Quick Setup
echo ==================================
echo.

REM Check if vcpkg exists
if exist "C:\vcpkg\vcpkg.exe" (
    set VCPKG=C:\vcpkg\vcpkg.exe
    echo Found vcpkg at C:\vcpkg
) else if exist "%USERPROFILE%\vcpkg\vcpkg.exe" (
    set VCPKG=%USERPROFILE%\vcpkg\vcpkg.exe
    echo Found vcpkg at %USERPROFILE%\vcpkg
) else (
    echo vcpkg not found. Please install vcpkg first:
    echo.
    echo   git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
    echo   cd C:\vcpkg
    echo   bootstrap-vcpkg.bat
    echo.
    pause
    exit /b 1
)

echo.
echo Installing required packages...
echo.

%VCPKG% install glfw3:x64-windows
%VCPKG% install glad:x64-windows
%VCPKG% install imgui[glfw-binding,opengl3-binding]:x64-windows

echo.
echo Integrating with Visual Studio...
%VCPKG% integrate install

echo.
echo ==================================
echo Setup Complete!
echo ==================================
echo.
echo Next steps:
echo   1. Close and reopen Visual Studio
echo   2. Rebuild your project (Ctrl+Shift+B)
echo   3. Run the application (F5)
echo.
pause
