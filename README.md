# 🏔️ Landscape Designer V0.7.8

A professional terrain generation and sculpting tool with real-time visualization, paint/sculpt capabilities, and game engine integration.

![Landscape Designer](https://img.shields.io/badge/version-0.7.8-success.svg)
![Platform](https://img.shields.io/badge/platform-Windows-blue.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

---

## ⚠️ Development Status

**This project is actively under development.** While core features are functional and tested, some features may contain bugs or incomplete functionality. 

**We welcome your feedback!** 🙌
- Found a bug? [Open an issue](https://github.com/dmorton63/Geo-Lab_Studio/issues)
- Have a feature suggestion? [Start a discussion](https://github.com/dmorton63/Geo-Lab_Studio/discussions)
- Want to contribute? Pull requests are welcome!

**Current Status:**
- ✅ Core terrain generation - **Stable**
- ✅ Paint/sculpt system - **Stable**
- ✅ Import/Export pipeline - **Stable** (V0.7.7-0.7.8)
- ✅ 3D visualization - **Stable**
- ✅ Erosion simulation - **Stable**
- 🚧 Modal mouse controls - **Planned** (V0.8.0)
- 🚧 Project save/load - **Planned**

See [TODO.md](TODO.md) for the complete roadmap.

---

## 🎉 What's New in V0.7.8

### 📤 Complete I/O Pipeline!
**Phase 2 (Import + Export) is now complete!**

- ✅ **Import RAW Heightmaps** (Ctrl+O) - 8-bit, 16-bit, 32-bit float
- ✅ **Export RAW Heightmaps** (Ctrl+S) - 8-bit, 16-bit, 32-bit float
- ✅ **Export PNG** (8-bit grayscale)
- ✅ **Round-trip editing** - Zero quality loss with 32-bit float format
- ✅ **Windows file dialogs** - Professional import/export workflow

**Complete Workflow:**  
World Machine/Gaea → Import (Ctrl+O) → Edit → Export (Ctrl+S) → Unreal/Unity ✨

### Previous Updates (V0.7.6-0.7.7)

#### 🐛 Bug Fixes
- **Tools panel no longer hides** - Fixed panel getting hidden behind viewports when clicking outside
- **Paint mode display fixed** - Auto-contrast now enabled automatically, no more dark display

### 🆕 New Project Dialog System
Professional project initialization workflow accessible via **File → New Project** (Ctrl+N)

**Project Setup Options:**
- **Target Engine** - Unreal Engine, Unity, Godot, or Custom
- **Terrain Type** - Plains, Hills, Mountains, Coastal, or Custom
- **World Size** - 512m, 1km, 2km, 4km, or 8km
- **Starting Template** - Perlin Noise (procedural) or Flat (blank canvas)

**Automatic Configuration:**
- Sets engine-appropriate height ranges
- Enables height clamping with calculated limits
- Generates initial terrain based on template choice
- Clears undo/redo for fresh start

### 🎨 Professional Menu Bar & Mode System
- **Global Menu Bar** - File, Edit, View, Tools, Help
- **Mode Toolbar** - Select, Paint, Sculpt*, Ramp*, Smooth* (*coming soon)
- **Contextual UI** - Parameter panel changes based on active mode

---

## ✨ Complete Feature Set

### 🎨 Paint & Sculpt System
- **4 Brush Types** - Raise, Lower, Smooth, Flatten
- **Adjustable Brushes** - Control radius and strength
- **Undo/Redo** - Full history (Ctrl+Z/Ctrl+Y, 10 levels)
- **Visual Brush Cursor** - See brush size in 2D view
- **Height Clamping** - Soft/hard modes to stay within engine limits

### 🎮 Game Engine Integration
- **Engine Presets** - Unreal Engine, Unity, Godot
- **Terrain Type Scaling** - Plains, Hills, Mountains, Coastal
- **Character-Aware Heights** - Prevents too-tall/short terrain
- **World Size Configuration** - 512m to 8km
- **Auto-Calculated Ranges** - Based on engine + type + size

### 🏔️ Terrain Generation
- **Perlin Noise System** - FBM, Ridged, Billowy, Classic modes
- **Procedural Generation** - Seed control for repeatability
- **Multiple Resolutions** - 128, 256, 512, 1024
- **Real-time Preview** - Async generation (non-blocking UI)
- **Flat Terrain Template** - Blank canvas for sculpting

### 🌊 Erosion Simulation
- **Hydraulic Erosion** - Water-based weathering
- **Thermal Erosion** - Talus angle simulation
- **Configurable Parameters** - Iterations, strength, rain, evaporation
- **Works on Any Terrain** - Generated or hand-painted

### 📊 Display & Visualization
- **5 Preview Modes** - Height, Slope, Curvature, Biome, Water
- **Auto-Contrast Display** - Visibility without data modification
- **3D Perspective View** - Real-time camera controls
- **Dual Viewport System** - 2D paint view + 3D preview
- **Height Scale Control** - Adjust 3D visualization scale

### 🎥 3D Camera System
- **Orbit Controls** - Left-click + drag to rotate
- **Pan Controls** - Right-click + drag to pan
- **Zoom Controls** - Mouse wheel
- **Camera Reset** - Home key or Edit menu
- **Live Camera Info** - Distance, yaw, pitch display

### 🔬 Analysis Layers
- **Slope Analysis** - Gradient calculation
- **Curvature Analysis** - Terrain shape
- **Water Mask** - Automatic water body detection
- **Biome Mask** - Slope-based terrain classification
- **Statistics Display** - Coverage percentages

### 💾 Export System
- **PNG Export** - 8-bit or 16-bit
- **RAW Export** - 32-bit float
- **Single Layer Export** - Current preview
- **All Layers Export** - Batch export all analysis layers
- **Timestamped Filenames** - Automatic naming

### 📊 Data Tools
- **Heightmap Data Dump** - Press D to dump to .txt
- **Sample Region Export** - 20x20 center sample
- **Viewport-Aware Dumps** - Knows which view was focused
- **Terrain Normalization** - Remap to specific range


## 🚀 Quick Start

### Prerequisites
- Windows 10/11
- Visual Studio 2022 (or compatible)
- OpenGL 3.3+ support

### Option 1: Automated Setup
```powershell
# Run from PowerShell in project directory
.\setup.ps1
```

### Option 2: Manual Setup
See [SETUP.md](SETUP.md) for detailed instructions.

### First Run
1. Launch the application
2. **File → New Project** (Ctrl+N)
3. Select your target engine (Unreal/Unity/Godot)
4. Choose terrain type and world size
5. Pick starting template (Perlin Noise or Flat)
6. Start creating!

---

## ⌨️ Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| **Ctrl+N** | New Project |
| **Ctrl+Z** | Undo (paint mode) |
| **Ctrl+Y** | Redo (paint mode) |
| **Home** | Reset Camera |
| **D** | Dump heightmap data |
| **Left-Click + Drag** | Paint (paint mode) or Rotate Camera (3D view) |
| **Right-Click + Drag** | Pan Camera (3D view) |
| **Mouse Wheel** | Zoom Camera (3D view) |

---

## 🎯 Typical Workflows

### Procedural Terrain Workflow
1. **File → New Project** → Select engine + Perlin Noise template
2. Adjust terrain parameters (noise type, octaves, scale)
3. Switch preview modes to analyze terrain
4. Apply erosion if desired
5. Switch to Paint mode for detail work
6. Export heightmap for game engine

### Sculpting Workflow
1. **File → New Project** → Select engine + Flat template
2. Switch to Paint mode
3. Use Raise/Lower brushes to sculpt terrain
4. Use Smooth brush to blend
5. Use Flatten brush for plateaus
6. Enable height clamping to stay in engine limits
7. Export heightmap

### Hybrid Workflow
1. Start with Perlin Noise template
2. Apply erosion for natural weathering
3. Switch to Paint mode
4. Hand-paint roads, paths, plateaus
5. Use Flatten brush for building sites
6. Normalize to final height range
7. Export all layers (height + analysis)


## 📁 Architecture

```
HeightMapGenerator/
├── main.cpp                 - Application entry, menu bar, mode system
├── UIPanel.h/cpp            - Contextual parameter interface
├── Renderer.h/cpp           - OpenGL dual viewport rendering
├── Framebuffer.h/cpp        - 2D/3D framebuffer management
├── LandscapeDesigner.h/cpp  - Terrain generation pipeline
├── LandscapeParameters.h    - All parameters + editor state
├── EngineScalingHelper.h    - Game engine integration
├── Image.h                  - 2D image container
└── LandscapeDesign.h        - Output structure & layer types
```

### Key Design Patterns
- **Mode-Based UI** - Context-sensitive parameter display
- **Dual Viewport** - Independent 2D paint + 3D preview
- **Async Generation** - Non-blocking terrain updates
- **Command Pattern** - Undo/redo for paint operations
- **Modular Pipeline** - Each function single-responsibility

---

## 🎨 Menu System Overview

### File Menu
- **New Project...** (Ctrl+N) - Project setup dialog
- **Import Heightmap...** (Ctrl+O) - *Coming in Phase 2.1*
- **Export Heightmap...** (Ctrl+S) - *Coming soon*
- **Exit** (Alt+F4) - Close application

### Edit Menu
- **Undo** (Ctrl+Z) - Undo last paint operation
- **Redo** (Ctrl+Y) - Redo previously undone operation
- **Reset Camera** (Home) - Reset 3D camera to default

### View Menu
- **3D Preview** - Toggle 3D perspective view
- **Auto-Contrast Display** - Toggle auto-contrast
- **Preview Mode** - Switch between Height/Slope/Curvature/Biome/Water

### Tools Menu
- **Apply Erosion** - Run erosion simulation
- **Normalize Terrain to Range** - Remap heights to current min/max

### Help Menu
- **About** - *Coming soon*

---

## 🎯 Mode System

### Select Mode (Default)
**Purpose:** View, navigate, and generate terrain

**Available Controls:**
- All terrain generation parameters
- Preview mode selection
- 3D view controls
- Erosion controls
- Statistics & export options

**Interactions:**
- 3D camera controls (when hovering 3D viewport)
- Parameter adjustments trigger regeneration
- No painting active

### Paint Mode
**Purpose:** Hand-paint heightmap details

**Available Controls:**
- Brush type selection (Raise, Lower, Smooth, Flatten)
- Brush radius/strength
- Height clamping controls
- Engine-aware presets
- Statistics & export options

**Interactions:**
- Left-click + drag to paint (2D viewport only)
- Visual brush cursor
- Undo/Redo support
- Auto-update disabled (manual painting)

### Future Modes (UI Prepared)
- **Sculpt Mode** - Advanced sculpting tools
- **Ramp Mode** - Create ramps/slopes
- **Smooth Mode** - Dedicated smoothing workflow


## 🎯 Roadmap

### ✅ Completed (V0.7.6)
- ✅ Professional 3-panel layout
- ✅ Menu bar & mode toolbar system
- ✅ New Project dialog with engine integration
- ✅ Paint/sculpt with undo/redo
- ✅ Dual viewport (2D paint + 3D preview)
- ✅ 3D camera controls (orbit, pan, zoom)
- ✅ 5 preview modes (Height, Slope, Curvature, Biome, Water)
- ✅ Erosion simulation (hydraulic + thermal)
- ✅ Export system (PNG, RAW, all layers)
- ✅ Auto-contrast display
- ✅ Height clamping for engine limits

### 🚧 In Progress (V0.8.0)
- **Modal Mouse Wheel System** - Advanced paint controls
  - Press R/S/F/L/H keys to set mouse wheel mode
  - Adjust Radius, Strength, Feather, Start Level, Max Height
  - Visual HUD with mode feedback
  - Fully designed, implementation ready

### 📅 Upcoming

**Phase 2.1 - Heightmap Import**
- Import PNG/RAW heightmaps
- Automatic resolution matching
- Preserve existing edits

**Phase 2.2 - Project Save/Load**
- Save complete project state
- Load previous sessions
- Parameter presets

**Phase 3 - Advanced Features**
- River generation
- Road/path tools
- Texture painting
- Custom brush shapes
- Layer system

**Phase 4 - Professional Tools**
- Batch generation
- Command-line interface
- Python scripting API
- Plugin system


## 🛠️ Technical Specifications

### Performance
- **Async Terrain Generation** - Non-blocking UI
- **Parallel Perlin Noise** - std::execution::par
- **Debounced Updates** - 150ms delay prevents excessive regeneration
- **Efficient Resampling** - Bilinear interpolation

### Build Status
- ✅ **0 Errors**
- ✅ **0 Warnings**
- ✅ **All Features Functional**

### Dependencies
- **GLFW 3.3+** - Windowing and input
- **OpenGL 3.3+** - Rendering
- **Dear ImGui** - UI framework
- **GLM** - Mathematics library
- **stb_image_write** - Image export
- **C++17** - Language standard

### Platform Support
- Windows 10/11 (primary)
- Visual Studio 2022 (recommended)
- Other platforms untested

---

## 📚 Documentation

- **[TODO.md](TODO.md)** - Development roadmap and task tracking
- **[COMPLETE_FEATURE_SUMMARY.md](COMPLETE_FEATURE_SUMMARY.md)** - Full feature list and version history
- **[V076_MENU_SYSTEM_IMPLEMENTATION.md](V076_MENU_SYSTEM_IMPLEMENTATION.md)** - Menu bar documentation
- **[MODAL_MOUSE_WHEEL_SYSTEM_SPEC.md](MODAL_MOUSE_WHEEL_SYSTEM_SPEC.md)** - V0.8.0 design specs
- **[3_PANEL_VISUAL_GUIDE.md](3_PANEL_VISUAL_GUIDE.md)** - Layout and design guide

---

## 💡 Usage Tips

### Getting the Best Results
1. **Start with New Project dialog** - Let the system configure engine-appropriate ranges
2. **Use appropriate world size** - Larger worlds need higher resolution
3. **Apply erosion after generation** - Natural weathering effect
4. **Paint in layers** - Start rough, refine with smaller brushes
5. **Enable height clamping** - Stay within engine limits automatically

### Performance Tips
- Use lower resolution (128-256) for experimentation
- Disable auto-update when making multiple parameter changes
- Use 1024 resolution only for final export
- 3D preview can be hidden for faster painting

### Engine-Specific Advice
- **Unreal Engine** - Use 2km world size for standard landscapes
- **Unity** - 1km works well for terrain chunks
- **Godot** - Start with 512m for testing

---

## 🤝 Contributing

**We welcome contributions!** This is an active project and we appreciate:
- 🐛 **Bug Reports** - Found an issue? Let us know!
- 💡 **Feature Suggestions** - Have an idea? Share it!
- 📝 **Documentation** - Improve guides, add examples
- 💻 **Code Contributions** - Fix bugs, add features

### High Priority Areas
- Additional starting templates (Hills, Mountains, Coastal presets)
- Texture painting system
- Advanced brush shapes (square, custom)
- More erosion algorithms
- Mac/Linux ports

### Medium Priority
- Additional game engine presets
- Performance optimizations
- UI themes
- Export format extensions (TIFF, EXR)

### How to Contribute
1. **Fork** the repository
2. **Create** a feature branch (`git checkout -b feature/AmazingFeature`)
3. **Make** your changes (follow existing code style)
4. **Test** thoroughly (ensure 0 errors, 0 warnings)
5. **Document** your changes (update README/TODO as needed)
6. **Commit** with descriptive messages (`git commit -m 'Add AmazingFeature'`)
7. **Push** to your branch (`git push origin feature/AmazingFeature`)
8. **Submit** a pull request

### Code Style Guidelines
- Follow existing C++ code style
- Use meaningful variable names
- Comment complex algorithms
- Keep functions focused (single responsibility)
- Test on Windows 10/11 with Visual Studio 2022

### Questions?
- Open an [issue](https://github.com/dmorton63/Geo-Lab_Studio/issues) for bugs
- Start a [discussion](https://github.com/dmorton63/Geo-Lab_Studio/discussions) for ideas
- Check [TODO.md](TODO.md) for current roadmap

**All contributions are appreciated!** 🙏

---

## 📄 License

MIT License - feel free to use in your projects!

See [LICENSE](LICENSE) for full details.

---

## 🙏 Acknowledgments

- **ImGui** by Omar Cornut - Excellent UI framework
- **GLFW** by Camilla Löwy - Cross-platform windowing
- **GLM** - OpenGL Mathematics library
- **stb_image_write** by Sean Barrett - Simple image export
- Inspired by procedural generation pioneers and professional terrain tools (World Machine, Gaea, Houdini)

**Special thanks to all contributors and testers!**

---

## 📊 Version History

- **V0.7.8** (Current) - Heightmap Export, Complete I/O Pipeline!
- **V0.7.7** - Heightmap Import
- **V0.7.6** - Menu bar, mode toolbar, New Project dialog, bug fixes
- **V0.7.5** - Auto-contrast, 3D camera controls, viewport focus tracking
- **V0.7.0** - Initial 3-panel layout
- **V0.6.x** - Erosion simulation system
- **V0.5.x** - Paint/sculpt implementation
- **V0.4.x** - Dual viewport system
- **V0.3.x** - Analysis layers
- **V0.2.x** - Core terrain generation

See [COMPLETE_FEATURE_SUMMARY.md](COMPLETE_FEATURE_SUMMARY.md) for detailed version history.

---

**Made with ❤️ for terrain enthusiasts and game developers**

**Build Date:** 2024  
**Status:** 🚀 Active Development  
**Current Focus:** Modal Mouse Wheel System (V0.8.0)
