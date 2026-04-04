# Project Structure & Dependencies

## File Organization

```
HeightMapGenerator/
│
├── Core Data Structures
│   ├── Image.h                      → Simple 2D float container
│   ├── LandscapeDesign.h            → Output structure (height, slope, curvature, etc.)
│   ├── LandscapeParameters.h        → Input parameters (sliders)
│   └── DisplayMode.h                → Enum for view modes (future)
│
├── Pipeline Logic
│   ├── LandscapeDesigner.h
│   └── LandscapeDesigner.cpp
│       └── Functions:
│           • normalize()            → [0, 1] normalization
│           • computeSlope()         → Gradient magnitude
│           • computeCurvature()     → Laplacian
│           • computeWaterMask()     → Threshold at waterLevel
│           • computeBiomeMask()     → Classify terrain types
│           • addNoise()             → Procedural variation
│           • designLandscape()      → Orchestrator function
│
├── Rendering System
│   ├── Renderer.h
│   └── Renderer.cpp
│       └── Responsibilities:
│           • OpenGL texture upload
│           • Shader compilation
│           • Quad rendering
│
├── User Interface
│   ├── UIPanel.h
│   └── UIPanel.cpp
│       └── ImGui Components:
│           • Parameter sliders
│           • Statistics display
│           • Auto-update toggle
│
└── Application Entry
    └── main.cpp
        └── Main Loop:
            • GLFW window management
            • ImGui initialization
            • Heightmap generation
            • Pipeline execution
            • Render loop
```

## Dependency Graph

```
                    ┌─────────────┐
                    │   main.cpp  │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
           ▼               ▼               ▼
    ┌──────────┐   ┌─────────────┐  ┌──────────┐
    │ UIPanel  │   │  Renderer   │  │Landscape │
    │          │   │             │  │ Designer │
    └────┬─────┘   └──────┬──────┘  └────┬─────┘
         │                │              │
         │                │              │
         ▼                ▼              ▼
    ┌─────────────────────────────────────────┐
    │         Landscape Structures            │
    ├─────────────────────────────────────────┤
    │ • LandscapeParameters                   │
    │ • LandscapeDesign                       │
    │ • Image                                 │
    │ • DisplayMode                           │
    └─────────────────────────────────────────┘
```

## External Dependencies

```
Project Files
     │
     ├─── GLAD (C:\glad)
     │    └── glad.h
     │    └── khrplatform.h
     │    └── glad.c (copied to project)
     │
     ├─── GLFW (via vcpkg)
     │    └── glfw3.h
     │    └── glfw3.lib
     │
     └─── ImGui (via vcpkg)
          ├── imgui.h
          ├── imgui_impl_glfw.h
          └── imgui_impl_opengl3.h
```

## Data Flow

```
User Input (Sliders)
       │
       ▼
[LandscapeParameters]
       │
       ▼
generateHeightmap()  ──────────────┐
       │                           │
       ▼                           │
[Raw Image Data]                   │
       │                           │
       ▼                           │
designLandscape() ◄────────────────┘
       │
       ├─► normalize()
       ├─► addNoise()
       ├─► computeSlope()
       ├─► computeCurvature()
       ├─► computeWaterMask()
       └─► computeBiomeMask()
                │
                ▼
       [LandscapeDesign]
                │
                ├─────────────────┐
                │                 │
                ▼                 ▼
         Renderer.uploadTexture()  UIPanel.render()
                │                      │
                ▼                      ▼
        OpenGL Display          Statistics Display
```

## Build Configuration

```
Compiler Settings:
├── Language: C++20
├── Platform: x64
└── Include Paths:
    ├── C:\glad\include
    └── $(VcpkgRoot)\installed\x64-windows\include (auto)

Linker Settings:
└── Libraries:
    ├── opengl32.lib
    ├── glfw3.lib (auto via vcpkg)
    └── imgui.lib (auto via vcpkg)
```

## Pipeline Flow (Detailed)

```
┌─────────────────────────────────────────────────────┐
│ 1. generateHeightmap(params)                        │
│    • Creates base terrain using sin/cos waves       │
│    • Frequency controlled by params.heightmapFreq   │
└──────────────────┬──────────────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────────────┐
│ 2. designLandscape(rawHeight, params)               │
└──────────────────┬──────────────────────────────────┘
                   │
    ┌──────────────┴────────────────┐
    │                               │
    ▼                               ▼
┌──────────┐                  ┌────────────┐
│normalize │                  │  addNoise  │
│ (0 to 1) │ ────────────────►│ (optional) │
└──────────┘                  └─────┬──────┘
                                    │
                                    ▼
                            ┌───────────────┐
                            │ computeSlope  │
                            │  (gradient)   │
                            └───────┬───────┘
                                    │
                                    ▼
                            ┌────────────────┐
                            │computeCurvature│
                            │  (Laplacian)   │
                            └───────┬────────┘
                                    │
                ┌───────────────────┴──────────────────┐
                │                                      │
                ▼                                      ▼
        ┌──────────────┐                    ┌──────────────────┐
        │computeWater  │                    │ computeBiomeMask │
        │    Mask      │ ──────────────────►│                  │
        │ (threshold)  │                    │ Water/Grass/Rock │
        └──────────────┘                    └──────────────────┘
                                                     │
                                                     ▼
                                            ┌─────────────────┐
                                            │LandscapeDesign  │
                                            │   (output)      │
                                            └─────────────────┘
```

---

## Key Features

✓ **Modular Design** - Each function is independent and testable
✓ **Real-time Updates** - Change parameters and see instant results
✓ **Expandable** - Easy to add new features (erosion, rivers, etc.)
✓ **Performance Ready** - Built for future SIMD optimization
✓ **Clean Separation** - Logic, rendering, and UI are decoupled
