# Quick Setup for HeightMapGenerator
# Using existing GLAD at C:\glad

Write-Host "==================================" -ForegroundColor Cyan
Write-Host "Landscape Designer Configuration" -ForegroundColor Cyan
Write-Host "==================================" -ForegroundColor Cyan
Write-Host ""

# 1. Copy GLAD source to project
Write-Host "Step 1: Copying GLAD source files..." -ForegroundColor Yellow

if (Test-Path "C:\glad\src\glad.c") {
    Copy-Item "C:\glad\src\glad.c" -Destination "." -Force
    Write-Host "  ✓ Copied glad.c" -ForegroundColor Green
} else {
    Write-Host "  ✗ glad.c not found at C:\glad\src\glad.c" -ForegroundColor Red
}

# 2. Check for vcpkg
Write-Host ""
Write-Host "Step 2: Checking for vcpkg and dependencies..." -ForegroundColor Yellow

$vcpkgPath = $null
$possiblePaths = @("C:\vcpkg", "$env:USERPROFILE\vcpkg", "D:\vcpkg")

foreach ($path in $possiblePaths) {
    if (Test-Path "$path\vcpkg.exe") {
        $vcpkgPath = $path
        Write-Host "  ✓ Found vcpkg at $path" -ForegroundColor Green
        break
    }
}

if ($null -eq $vcpkgPath) {
    Write-Host "  ⚠ vcpkg not found. Installing to C:\vcpkg..." -ForegroundColor Yellow
    
    git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
    Set-Location C:\vcpkg
    .\bootstrap-vcpkg.bat
    Set-Location D:\quantum\HeightMapGenerator
    
    $vcpkgPath = "C:\vcpkg"
}

# 3. Install dependencies
Write-Host ""
Write-Host "Step 3: Installing GLFW and ImGui..." -ForegroundColor Yellow

& "$vcpkgPath\vcpkg.exe" install glfw3:x64-windows
& "$vcpkgPath\vcpkg.exe" install imgui[glfw-binding,opengl3-binding]:x64-windows

Write-Host "  ✓ Dependencies installed" -ForegroundColor Green

# 4. Integrate with Visual Studio
Write-Host ""
Write-Host "Step 4: Integrating with Visual Studio..." -ForegroundColor Yellow
& "$vcpkgPath\vcpkg.exe" integrate install
Write-Host "  ✓ Integration complete" -ForegroundColor Green

# 5. Summary
Write-Host ""
Write-Host "==================================" -ForegroundColor Green
Write-Host "Configuration Complete!" -ForegroundColor Green
Write-Host "==================================" -ForegroundColor Green
Write-Host ""
Write-Host "Project paths configured:" -ForegroundColor Cyan
Write-Host "  • GLAD: C:\glad" -ForegroundColor White
Write-Host "  • vcpkg: $vcpkgPath" -ForegroundColor White
Write-Host "  • Project: D:\quantum\HeightMapGenerator" -ForegroundColor White
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Cyan
Write-Host "  1. Add glad.c to your project in Visual Studio" -ForegroundColor White
Write-Host "  2. Build the project (Ctrl+Shift+B)" -ForegroundColor White
Write-Host "  3. Run (F5)" -ForegroundColor White
Write-Host ""
Write-Host "If you get include errors, the project file has been" -ForegroundColor Yellow
Write-Host "updated with the correct paths automatically." -ForegroundColor Yellow
Write-Host ""
