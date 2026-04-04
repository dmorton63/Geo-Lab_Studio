# 🏔️ Landscape Designer V0.2

An interactive, real-time terrain generation tool with adjustable parameters and live visualization.

![Landscape Designer](https://img.shields.io/badge/status-active-success.svg)
![Platform](https://img.shields.io/badge/platform-Windows-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

## ✨ Features

### 🎛️ Real-Time Parameter Adjustment
- **Water Level** - Control water/land threshold
- **Slope Threshold** - Rock vs grass classification
- **Noise Amount** - Add procedural variation
- **Heightmap Frequency** - Terrain wave patterns
- **Resolution** - 128px to 1024px maps
- **Erosion Strength** - (Coming soon!)

### 📊 Live Statistics
- Biome distribution (Water/Grass/Rock)
- Pixel counts and percentages
- Real-time updates

### 🎨 Visualization
- Grayscale heightmap display
- OpenGL-accelerated rendering
- Instant parameter feedback

## 🚀 Quick Start

### Option 1: Automated Setup
```powershell
# Run from PowerShell in project directory
.\setup.ps1
```

### Option 2: Manual Setup
See [SETUP.md](SETUP.md) for detailed instructions.

## 📁 Architecture

```
HeightMapGenerator/
├── Image.h                  - Simple 2D image container
├── LandscapeDesign.h        - Output structure & biome types
├── LandscapeDesigner.h/cpp  - Core pipeline (5 functions)
├── LandscapeParameters.h    - Adjustable parameters
├── Renderer.h/cpp           - OpenGL visualization
├── UIPanel.h/cpp            - ImGui interface
└── main.cpp                 - Application entry & main loop
```

## 🧩 Core Pipeline

The landscape designer uses a clean, modular pipeline:

```cpp
LandscapeDesign designLandscape(const Image& rawHeight, const LandscapeParameters& params) {
    out.height     = normalize(rawHeight);
    out.height     = addNoise(out.height, params.noiseAmount);
    out.slope      = computeSlope(out.height);
    out.curvature  = computeCurvature(out.height);
    out.waterMask  = computeWaterMask(out.height, params.waterLevel);
    out.biomeMask  = computeBiomeMask(out.slope, out.waterMask, params.slopeThreshold);
    return out;
}
```

Each function is:
- ✅ Independently testable
- ✅ Single responsibility
- ✅ No hidden state
- ✅ Easy to optimize (SIMD-ready)

## 🎯 Roadmap

### V0.3 - Visualization Modes
- [ ] Display mode switcher (Height/Slope/Curvature/Biome)
- [ ] Color-coded biome visualization
- [ ] Heatmap for slope/curvature
- [ ] 3D preview mode

### V0.4 - Advanced Generation
- [ ] Hydraulic erosion simulation
- [ ] Multi-octave Perlin noise
- [ ] Thermal erosion
- [ ] River generation

### V0.5 - Export & Integration
- [ ] Export heightmaps (PNG, RAW, etc.)
- [ ] Save/load parameter presets
- [ ] Batch generation
- [ ] Engine integration examples

## 🛠️ Dependencies

- **GLFW 3.3+** - Windowing
- **GLAD** - OpenGL loader
- **Dear ImGui** - UI framework
- **C++17** - Language standard

## 📝 Usage Example

```cpp
// Create parameters
LandscapeParameters params;
params.waterLevel = 0.4f;
params.slopeThreshold = 0.05f;
params.noiseAmount = 0.1f;
params.heightmapFrequency = 10.0f;

// Generate heightmap
Image rawHeight = generateHeightmap(params);

// Design landscape
LandscapeDesign design = LandscapeDesigner::designLandscape(rawHeight, params);

// Access results
Image& heightMap = design.height;
Image& biomeMap = design.biomeMask;
```

## 🤝 Contributing

Contributions welcome! Areas of interest:
- Erosion algorithms
- Noise generation methods
- Visualization improvements
- Performance optimization

## 📄 License

MIT License - feel free to use in your projects!

## 🙏 Acknowledgments

- ImGui by Omar Cornut
- GLFW by Camilla Löwy
- Inspired by procedural generation pioneers

---

**Made with ❤️ for terrain enthusiasts**
