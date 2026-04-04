# **TODO.md — Terrain Editor Development Roadmap**
### *Landscape Designer V0.5 → V1.0*

---

## **✅ PHASE 0: Foundation (COMPLETE)**

### **Core Architecture**
- [x] Heightmap as single source of truth (`Image` class)
- [x] Multi-view system (2D Paint / 3D View)
- [x] Real-time view synchronization
- [x] Event-driven update pipeline
- [x] Centralized heightmap storage
- [x] Efficient mesh regeneration (`TerrainMesh`)
- [x] Undo/redo stack (10 levels, Ctrl+Z/Ctrl+Y)
- [x] Resolution resampling (preserves edits)

### **Terrain Generation**
- [x] Perlin noise generator (`PerlinNoise`)
- [x] Multiple noise types (Classic, FBM, Ridged, Billowy)
- [x] Configurable octaves, persistence, lacunarity
- [x] Radial gradient blending
- [x] Async generation (non-blocking UI)

### **Terrain Analysis**
- [x] Slope computation (parallel)
- [x] Curvature computation (parallel)
- [x] Water mask detection
- [x] Biome classification (Water/Grass/Rock)

### **Editing Tools**
- [x] Paint mode with brush cursor
- [x] Raise brush
- [x] Lower brush
- [x] Smooth brush (3x3 neighborhood)
- [x] Flatten brush
- [x] Configurable brush radius & strength

### **Rendering**
- [x] 2D quad renderer (grayscale/heatmap modes)
- [x] 3D terrain renderer with lighting
- [x] Camera controls (orbit, pan, zoom)
- [x] Normal calculation (per-vertex averaging)
- [x] Preview modes (Height/Slope/Curvature/Biome/Water)

### **Erosion Simulation**
- [x] Hydraulic erosion
- [x] Thermal erosion
- [x] Configurable parameters (iterations, strength, rain, etc.)

### **Developer Tools**
- [x] Debug data dumper (heightmap samples to .txt)
- [x] Coordinate system verification
- [x] Real-time statistics panel

---

## **🚨 PHASE 1: Critical Brush Safety (IMMEDIATE)** ⚠️
*Priority: CRITICAL*** - Prevent terrain catastrophes*

### **1.1 Height Clamping**
- [ ] **User-configurable min/max height limits** (default: 0.0 to 1.0)
- [ ] **Apply clamping to all brushes:**
  - [ ] Raise brush (prevent pushing above max)
  - [ ] Lower brush (prevent digging below min)
  - [ ] Flatten brush (clamp target height)
- [ ] **Visual feedback:**
  - [ ] Show current height value under cursor
  - [ ] Warning indicator when approaching limits
  - [ ] Change brush cursor color when at limits (e.g., red)
- [ ] **Clamping modes:**
  - [ ] Hard clamp (immediate stop at limit)
  - [ ] Soft clamp (ease-in near limits, preserve natural feel)
  - [ ] Per-brush override (some brushes ignore clamps if needed)
- [ ] **UI controls:**
  - [ ] Min/Max sliders in Parameters panel
  - [ ] Quick presets (0.0-1.0, 0.1-0.9, 0.2-0.8)
  - [ ] Enable/disable clamping checkbox

**Why Critical:** Without clamping, accidental brush strokes can push terrain to extreme values (>1.0 or <0.0), breaking rendering, causing NaN/Inf errors, and making undo insufficient for recovery. This is a usability safety net.

---

## **🔨 PHASE 2: Import/Export Pipeline (NEXT)**
*Priority: High - Workflow integration*

### **2.1 Heightmap Import**
- [ ] Load 16-bit RAW files
- [ ] Load PNG (8-bit and 16-bit grayscale)
- [ ] Load EXR (floating-point)
- [ ] Auto-detect min/max elevation
- [ ] Normalize to [0.0, 1.0] range
- [ ] Import dialog with format selection
- [ ] Drag-and-drop support (future)

### **2.2 Heightmap Export**
- [ ] Export RAW (16-bit)
- [ ] Export PNG (16-bit grayscale)
- [ ] Export EXR (32-bit float)
- [ ] Preserve full precision range
- [ ] Export dialog with format selection

### **2.3 Metadata Export**
- [ ] Export slope map
- [ ] Export curvature map
- [ ] Export biome mask
- [ ] Export water mask
- [ ] Bundle as multi-channel texture (optional)

---

## **🎯 PHASE 3: Enhanced Editing Tools**
*Priority: Medium - Complete the brush toolset*

### **3.1 Additional Brushes**
- [ ] Noise brush (add procedural detail)
- [ ] Blend brush (smooth between regions)
- [ ] Terrace brush (stepped plateaus)
- [ ] Erosion brush (apply local erosion)

### **3.2 Brush Improvements**
- [ ] Pressure sensitivity (tablet support)
- [ ] Brush presets (save/load configurations)
- [ ] Brush shape options (circular, square, custom)
- [ ] Brush jitter/scatter
- [ ] Invert mode (Shift+Click)

### **3.3 Stamp System**
- [ ] Load grayscale stamps (PNG)
- [ ] Stamp placement with preview
- [ ] Stamp rotation (0-360°)
- [ ] Stamp scale
- [ ] Blending modes (Add, Subtract, Multiply, Replace)
- [ ] Stamp library panel
- [ ] Save user-created stamps

### **3.4 Region Masks**
- [ ] Box selection tool
- [ ] Lasso selection tool
- [ ] Paint selection brush
- [ ] Invert selection
- [ ] Grow/Shrink selection
- [ ] Save/load selection masks
- [ ] Apply brush only to selected regions

---

## **📊 PHASE 4: Advanced Terrain Analysis**
*Priority: Medium - For smart scaling & game design*

### **4.1 Feature Detection**
- [ ] Local maxima/minima (peaks/valleys)
- [ ] Watershed detection
- [ ] Ridge detection
- [ ] Plateau detection
- [ ] Steep slope detection (cliffs)

### **4.2 Terrain Classification**
- [ ] Plains (low slope, low elevation)
- [ ] Hills (medium slope, medium elevation)
- [ ] Mountains (high slope, high elevation)
- [ ] Rivers/Valleys (concave curvature)
- [ ] Basins (local minima)
- [ ] Ridges/Peaks (local maxima)

### **4.3 Feature Subdivision**
- [ ] Size classification (Small/Medium/Large/Mega)
- [ ] Feature boundary detection
- [ ] Feature visualization overlay

---

## **⚙️ PHASE 5: Engine-Aware Scaling System**
*Priority: Medium - Game integration focus*

### **5.1 Engine Presets**
- [ ] Target engine dropdown (Unreal/Unity/Godot/Custom)
- [ ] Load engine defaults:
  - [ ] Character height (cm/m)
  - [ ] Capsule radius
  - [ ] Walkable slope angle
  - [ ] World unit scale
  - [ ] Default terrain size
- [ ] Character-relative scaling

### **5.2 Smart Scaling UI**
- [ ] Category-based multiplier panel
- [ ] Min/Max multiplier per terrain type
- [ ] Global scale slider
- [ ] Real-time preview
- [ ] Non-uniform scaling (preserve detail)

### **5.3 Walkability Analysis**
- [ ] Walkability heatmap (based on slope)
- [ ] Climbable surface detection
- [ ] Navigation mesh preview (future)

---

## **🌐 PHASE 6: Spline Tools (Future)**
*Priority: Low - Advanced workflow*

### **6.1 Spline System**
- [ ] Catmull-Rom or Kochanek-Bartels curves
- [ ] Place control points
- [ ] Spline visualization (3D)
- [ ] Edit tangents

### **6.2 Spline-Based Deformation**
- [ ] Road carving tool
- [ ] River carving tool
- [ ] Plateau/Mesa tool
- [ ] Spline-based stamp placement

---

## **📦 PHASE 7: Object Placement (Future)**
*Priority: Low - Scene composition*

### **7.1 FBX Import**
- [ ] Load FBX meshes (Assimp or FBX SDK)
- [ ] Upload to GPU
- [ ] Render in 3D view with lighting

### **7.2 Placement Tools**
- [ ] Translate/Rotate/Scale gizmo
- [ ] Snap to terrain surface
- [ ] Align to terrain normal
- [ ] Scatter tool (place multiple objects)
- [ ] Biome-aware scatter (trees on grass, rocks on cliffs)

### **7.3 Export**
- [ ] Export object transforms (CSV/JSON)
- [ ] Export scene to UE/Unity/Godot format

---

## **🚀 PHASE 8: Advanced Features (Future)**

### **8.1 Erosion Enhancements**
- [ ] Sediment visualization
- [ ] Erosion animation (time-lapse)
- [ ] Wind erosion
- [ ] Glacial erosion

### **8.2 World Events**
- [ ] Tectonic uplift simulation
- [ ] Flooding simulation
- [ ] Volcanic deformation
- [ ] Earthquake faults

### **8.3 Multi-Layer Editing**
- [ ] Layer stack (Photoshop-style)
- [ ] Layer blending modes
- [ ] Layer masks
- [ ] World Composer mode (tile-based large worlds)

### **8.4 Camera Enhancements**
- [ ] Player-height preview camera
- [ ] Walk-through mode
- [ ] Flythrough paths
- [ ] Screenshot tool

---

## **🐛 KNOWN ISSUES & FIXES**

### **Recently Fixed**
- [x] Coordinate flip between Paint and 3D view *(Fixed: X-axis inversion)*
- [x] Texture color artifact *(Fixed: Texture coordinate alignment)*
- [x] Dark lighting after coordinate flip *(Fixed: Normal calculation)*

### **Current Issues**
- [ ] *(Add issues as they're discovered)*

---

## **🎨 UI/UX Improvements**

### **Immediate**
- [ ] Tool icons for brushes
- [ ] Keyboard shortcuts panel
- [ ] Tooltips on all UI elements
- [ ] Status bar (resolution, cursor position, height value)

### **Future - Dockable Window System**
*Priority: High - Major workflow improvement*

**Goal:** Transform from overlay panels to fully dockable multi-window layout (like Unity/Unreal/Blender)

- [ ] Enable ImGui docking (requires ImGui docking branch or ImGui 1.80+)
- [ ] Separate windows for each view:
  - [ ] **Editor View Window** (2D Paint mode)
  - [ ] **3D View Window** (perspective renderer)
  - [ ] **Parameters Window** (sliders, dropdowns, generation controls)
  - [ ] **Tools Window** (brush selection, stamp library)
  - [ ] **Statistics Window** (biome stats, performance metrics)
  - [ ] **Layers/History Window** (undo stack visualization)
- [ ] Docking framework:
  - [ ] Drag-and-drop window docking
  - [ ] Tab groups (multiple windows in same dock space)
  - [ ] Split horizontal/vertical
  - [ ] Undock to floating windows
  - [ ] Maximize/restore docked windows
- [ ] Layout management:
  - [ ] Save/load custom layouts
  - [ ] Layout presets (Default, Paint Focus, 3D Focus, Analysis)
  - [ ] Reset to default layout
- [ ] Update messaging system:
  - [ ] **Manual Update Mode**: 3D view updates only on "Apply" button
  - [ ] **Auto Update Mode**: 3D view updates in real-time (current behavior)
  - [ ] Toggle between modes in preferences
- [ ] Multi-monitor support:
  - [ ] Detach windows to separate monitors
  - [ ] Remember window positions across sessions

**Technical Notes:**
- ImGui has native docking support in `docking` branch or versions ≥1.80
- Enable with `ImGuiConfigFlags_DockingEnable` and `ImGuiConfigFlags_ViewportsEnable`
- Each window becomes a separate ImGui window with docking flags
- Requires refactoring current `UIPanel::render()` into separate window functions

### **Future - Other UI**
- [ ] Dark/Light theme toggle
- [ ] Mini-map (2D overview while in 3D)
- [ ] Context menus (right-click on terrain)
- [ ] Radial menus for brush selection (optional)

---

## **📚 Documentation Needs**

- [ ] User manual (basic workflow)
- [ ] API documentation (for future plugins)
- [ ] Tutorial videos (optional)
- [ ] Keyboard shortcut reference card
- [ ] Engine export guides (UE/Unity/Godot)

---

## **🔧 Build & Distribution**

- [ ] Installer creation (NSIS or WiX)
- [ ] Auto-update system (future)
- [ ] Crash reporting (future)
- [ ] Performance profiling (identify bottlenecks)

---

## **📝 Notes**

**Current Version:** V0.5 (Complete Edition)  
**Target Version:** V1.0 (Production Ready)

**Dependencies:**
- GLAD (OpenGL loader)
- GLFW3 (windowing)
- ImGui (UI)
- GLM (math)
- stb_image (future: image I/O)
- Assimp (future: FBX import)

**Development Environment:**
- Visual Studio 2022
- vcpkg for dependency management
- C++17 standard

---

*Last Updated: 2025*  
*Generated by: GitHub Copilot + Developer Review*

# Option 1: Pull and merge (recommended)
git pull origin main --allow-unrelated-histories

# If there are merge conflicts in README.md, resolve them:
# 1. Open README.md in Visual Studio
# 2. Keep your local version or merge both
# 3. Then:
git add .
git commit -m "Merge remote README with local project"

# Now push:
git push -u origin main
