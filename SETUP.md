# Landscape Designer V0.2 - Setup Guide

## Required Libraries

This project requires the following libraries:
1. **GLFW** - Window and input management
2. **GLAD** - OpenGL function loader
3. **Dear ImGui** - Immediate mode GUI

## Installation via vcpkg (Recommended)

### Step 1: Install vcpkg
```powershell
# Clone vcpkg if you don't have it
cd D:\
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```

### Step 2: Install Dependencies
```powershell
# From your vcpkg directory
.\vcpkg install glfw3:x64-windows
.\vcpkg install glad:x64-windows
.\vcpkg install imgui[glfw-binding,opengl3-binding]:x64-windows
```

### Step 3: Integrate with Visual Studio
```powershell
.\vcpkg integrate install
```

This will automatically configure Visual Studio to find these libraries.

## Alternative: Manual Setup

### GLFW
1. Download from https://www.glfw.org/download.html
2. Extract and add to project include/lib paths

### GLAD
1. Generate from https://glad.dav1d.de/
   - Language: C/C++
   - API gl: Version 3.3+
   - Profile: Core
   - Generate a loader: Yes
2. Add glad.c to your project
3. Add include files to project

### ImGui
1. Download from https://github.com/ocornut/imgui
2. Add these files to your project:
   - imgui.cpp
   - imgui_demo.cpp
   - imgui_draw.cpp
   - imgui_tables.cpp
   - imgui_widgets.cpp
   - backends/imgui_impl_glfw.cpp
   - backends/imgui_impl_opengl3.cpp

## Project Configuration

### Include Directories
Add these to your project:
- `$(VcpkgRoot)installed\x64-windows\include`

### Library Directories
- `$(VcpkgRoot)installed\x64-windows\lib`

### Linker Input
- glfw3.lib
- opengl32.lib
- imgui.lib

## Build and Run

After setup:
1. Open HeightMapGenerator.sln in Visual Studio
2. Build (Ctrl+Shift+B)
3. Run (F5)

You should see a window with:
- Real-time heightmap visualization
- Adjustable sliders for all parameters
- Live statistics

## Features

### Adjustable Parameters
- **Water Level**: Controls the height threshold for water
- **Slope Threshold**: Determines rock vs grass classification
- **Noise Amount**: Adds procedural noise to heightmap
- **Heightmap Frequency**: Controls the wave frequency of terrain
- **Resolution**: Change map size (128, 256, 512, 1024)

### Display Modes (Future)
- Height map (grayscale)
- Slope visualization
- Curvature display
- Biome mask (colored)

## Next Steps

To extend this further:
1. Add display mode switching (height/slope/biome views)
2. Implement erosion simulation
3. Add texture export functionality
4. Multi-layer noise generation
5. Real-time erosion preview
