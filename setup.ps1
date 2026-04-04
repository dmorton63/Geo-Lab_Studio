# Quick Setup Script for Landscape Designer
# Run this in PowerShell from your project directory

Write-Host "==================================" -ForegroundColor Cyan
Write-Host "Landscape Designer - Quick Setup" -ForegroundColor Cyan
Write-Host "==================================" -ForegroundColor Cyan
Write-Host ""

# Check if vcpkg is installed
$vcpkgPath = "C:\vcpkg"
$altVcpkgPath = "$env:USERPROFILE\vcpkg"

if (Test-Path $vcpkgPath) {
    Write-Host "Found vcpkg at $vcpkgPath" -ForegroundColor Green
    $vcpkg = "$vcpkgPath\vcpkg.exe"
} elseif (Test-Path $altVcpkgPath) {
    Write-Host "Found vcpkg at $altVcpkgPath" -ForegroundColor Green
    $vcpkg = "$altVcpkgPath\vcpkg.exe"
} else {
    Write-Host "vcpkg not found. Installing to C:\vcpkg..." -ForegroundColor Yellow
    
    git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
    Set-Location C:\vcpkg
    .\bootstrap-vcpkg.bat
    
    $vcpkg = "C:\vcpkg\vcpkg.exe"
}

Write-Host ""
Write-Host "Installing required packages..." -ForegroundColor Yellow

# Install dependencies
& $vcpkg install glfw3:x64-windows
& $vcpkg install glad:x64-windows
& $vcpkg install imgui[glfw-binding,opengl3-binding]:x64-windows

Write-Host ""
Write-Host "Integrating with Visual Studio..." -ForegroundColor Yellow
& $vcpkg integrate install

Write-Host ""
Write-Host "==================================" -ForegroundColor Green
Write-Host "Setup Complete!" -ForegroundColor Green
Write-Host "==================================" -ForegroundColor Green
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Cyan
Write-Host "1. Close and reopen Visual Studio" -ForegroundColor White
Write-Host "2. Rebuild your project (Ctrl+Shift+B)" -ForegroundColor White
Write-Host "3. Run the application (F5)" -ForegroundColor White
Write-Host ""
