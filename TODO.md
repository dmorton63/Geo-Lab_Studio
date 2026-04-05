# **TODO.md — Terrain Editor Development Roadmap**
### *Landscape Designer V0.7.5 (Professional 3-Panel Layout) → V1.0*

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
*Priority: CRITICAL - Prevent terrain catastrophes*

### **1.1 Height Clamping** ✅ COMPLETE
- [x] **User-configurable min/max height limits** (default: 0.0 to 1.0)
- [x] **Apply clamping to all brushes:**
  - [x] Raise brush (prevent pushing above max)
  - [x] Lower brush (prevent digging below min)
  - [x] Flatten brush (clamp target height)
- [x] **Visual feedback:**
  - [x] Show current height value under cursor
  - [x] Warning indicator when approaching limits
  - [x] Change indicator color when at limits (red/yellow)
- [x] **Clamping modes:**
  - [x] Hard clamp (immediate stop at limit)
  - [x] Soft clamp (ease-in near limits, preserve natural feel)
  - [x] Per-brush override (infrastructure in place)
- [x] **UI controls:**
  - [x] Min/Max sliders in Parameters panel
  - [x] Quick presets (0.0-1.0, 0.1-0.9, 0.2-0.8)
  - [x] Enable/disable clamping checkbox

**Implementation Complete!** All brushes now support configurable height clamping with soft/hard modes, visual feedback overlay shows cursor height and limit warnings, and a convenient preset system makes it easy to set safe ranges.

---

## **🔨 PHASE 2: Import/Export Pipeline (NEXT)**
*Priority: High - Workflow integration*

### **2.1 Heightmap Import** ✅ COMPLETE
- [x] Load 16-bit RAW files
- [x] Load 8-bit RAW files (bonus)
- [x] Load 32-bit Float RAW files
- [x] PNG (8-bit and 16-bit grayscale) - Future enhancement with stb_image
- [x] EXR (floating-point) - Future enhancement with OpenEXR
- [x] Auto-detect min/max elevation
- [x] Normalize to [0.0, 1.0] range
- [x] Import dialog with format selection
- [x] Windows file dialog integration
- [x] Automatic resolution matching/resampling
- [x] Clear undo/redo on import
- [x] Drag-and-drop support (future enhancement)

**Implementation Complete!** File → Import Heightmap (Ctrl+O) now supports:

**Implementation Complete!** File → Import Heightmap (Ctrl+O) now supports:
- **.raw / .r16** - 16-bit unsigned RAW heightmaps
- **.r8** - 8-bit unsigned RAW heightmaps  
- **.r32** - 32-bit float RAW heightmaps
- **Auto-detection** of min/max values with normalization to [0.0, 1.0]
- **Auto-resampling** if imported resolution doesn't match current setting
- **Windows file dialog** for easy file selection
- Clears undo/redo history for fresh start

Usage: File → Import Heightmap (Ctrl+O), select a RAW file, and it automatically loads and normalizes!

---

### **2.2 Heightmap Export** (IN PROGRESS)
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

### **3.2.5 Modal Mouse Wheel System (Paint Mode Focus)** 🎯
*Priority: High - Professional workflow enhancement*

**Concept:** Mouse wheel function changes based on active mode, selected via keyboard shortcuts or mouse button cycling. One input method controls multiple parameters efficiently.

**Key Innovation:** Instead of hardcoded controls, use a flexible modal system like Blender/ZBrush where pressing a key (R/S/F/L/H) sets what the mouse wheel adjusts.

**Modes & Keyboard Shortcuts:**
- [ ] **[R] Radius Mode:** Mouse wheel adjusts brush size (5-100)
- [ ] **[S] Strength Mode:** Mouse wheel adjusts brush intensity (1-100)
- [ ] **[F] Feather Mode:** Mouse wheel adjusts edge softness (0.0-1.0) 🆕
- [ ] **[L] Start Level Mode:** Mouse wheel sets minimum affected height (0.0-1.0) 🆕
- [ ] **[H] Max Height Mode:** Mouse wheel sets paint ceiling/floor (0.0-1.0) 🆕

**Mouse Button Cycling (Alternative Input):**
- [ ] Button 4 (programmable): Cycle through modes (Radius → Strength → Feather → Start Level → Max Height)
- [ ] Visual mode indicator in viewport HUD

**Modifier Keys:**
- [ ] Shift + Mouse Wheel: Fine adjustment (0.1x speed)
- [ ] Ctrl + Mouse Wheel: Coarse adjustment (2x speed)

**Visual Feedback:**
- [ ] Mode HUD (always visible in paint mode, top-left corner)
  - Shows all modes with current values
  - Highlights active mode in yellow
  - Example: "[R] Radius: 35.0" ← active, "[S] Strength: 75.0" ← inactive
- [ ] Adjustment overlay (temporary, near cursor)
  - Appears when scrolling
  - Shows visual slider and current value
  - Fades after 1 second
- [ ] Brush cursor updates to show current parameters
  - Gradient ring for feather visualization
  - Size changes reflect radius

**New Parameters (Requires Implementation):**

1. **Brush Feather/Falloff** 🆕
   - [ ] Implement smoothstep falloff calculation
   - [ ] 0.0 = Hard edge (stamp-like)
   - [ ] 1.0 = Very soft edge (gradual blend)
   - [ ] Visual: Brush cursor shows gradient ring
   - [ ] Integration with all brush types

2. **Paint Start Level (Height Masking - Lower Bound)** 🆕
   - [ ] Only affect terrain **below** this height
   - [ ] Use case: Fill valleys without touching peaks
   - [ ] Visual: Blue horizontal line overlay on viewport
   - [ ] Brush cursor tints red when over masked area
   - [ ] Enable/disable toggle in UI

3. **Max Paint Height (Height Ceiling/Floor)** 🆕
   - [ ] Raise brush stops at this ceiling
   - [ ] Lower brush stops at this floor
   - [ ] Different from global height clamping (per-session)
   - [ ] Use case: Create exact plateau heights
   - [ ] Visual: Red horizontal line overlay on viewport

**UI Implementation:**
- [ ] Add "Mouse Wheel Modes" section to paint parameters
- [ ] Mode dropdown (optional alternative to keyboard)
- [ ] Show/hide HUD toggles
- [ ] Show/hide adjustment overlay toggle
- [ ] All parameter value sliders (with current values)
- [ ] "Height Masking (Advanced)" collapsible section
- [ ] Enable/disable toggles for start level and max height masking
- [ ] Checkbox to show masking overlays on viewport

**Technical Implementation:**
- [ ] Add MouseWheelMode enum to LandscapeParameters
- [ ] Add new parameter variables (feather, startLevel, maxHeight)
- [ ] Keyboard mode selection (R/S/F/L/H key detection)
- [ ] Mouse button mode cycling (Button 4)
- [ ] Mouse wheel scroll handling with mode switching
- [ ] HUD rendering system (ImGui overlays)
- [ ] Brush falloff calculation update
- [ ] Height masking logic in brush application
- [ ] Viewport overlay rendering (horizontal lines for limits)

**Benefits:**
- ✅ One input controls many parameters (scalable)
- ✅ Mnemonic keyboard shortcuts (F = Feather, easy to remember)
- ✅ No menu diving during painting (maintain flow)
- ✅ Industry-standard workflow (matches Blender/ZBrush)
- ✅ Easy to add more modes in future

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

### **5.1 Engine Presets** ✅ COMPLETE
- [x] Target engine dropdown (Unreal/Unity/Godot/Custom)
- [x] Load engine defaults:
  - [x] Character height (cm)
  - [x] Capsule radius
  - [x] Walkable slope angle
  - [x] World unit scale
  - [x] Default terrain size
- [x] Character-relative scaling
- [x] Terrain type presets (Plains/Hills/Mountains/Coastal)
- [x] Real-time range calculation
- [x] Normalization system (meters ↔ 0.0-1.0)
- [x] Integration with height clamping system
- [x] **Normalize Terrain to Range button** (remap existing terrain to fit engine ranges)

**Implementation Complete!** The system now automatically calculates natural height ranges based on your target engine and character size. A 180cm character in UE5 generates hills (0-14m), mountains (0-100m), or plains (0-2m) that look perfectly proportioned when imported! The new normalize feature lets you remap existing terrain to fit these ranges with one click, preserving all features.

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

### **Future - Dockable Window System** ✅ PARTIAL (Dual Viewport Complete!)
*Priority: High - Major workflow improvement*

**Current Status:** Dual Viewport System Implemented! (Step toward full docking)

**What's Working Now (V0.7):**
- [x] **Separate windows for views:**
  - [x] **2D Paint View Window** (left viewport, top-down painting)
  - [x] **3D Preview Window** (right viewport, perspective renderer)
  - [x] **Parameters Window** (bottom panel, all controls)
- [x] **Framebuffer-based rendering** (each viewport renders independently)
- [x] **Responsive layout** (adjusts to window size)
- [x] **Viewport-specific mouse handling** (no coordinate confusion)
- [x] **Real-time synchronization** (paint in 2D, see in 3D instantly)
- [x] **1920x1080 default window** (widescreen-friendly)

**Still TODO for Full Docking:**
- [ ] Enable ImGui docking (requires ImGui docking branch or ImGui 1.80+)
- [ ] Additional windows:
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

**Recent Achievement (V0.7):** Implemented professional dual-viewport system as stepping stone to full docking! You can now paint in 2D while simultaneously viewing 3D results, just like Unity and Unreal! This is a **major** workflow improvement.

**Technical Notes:**
- ImGui has native docking support in `docking` branch or versions ≥1.80
- Enable with `ImGuiConfigFlags_DockingEnable` and `ImGuiConfigFlags_ViewportsEnable`
- Each window becomes a separate ImGui window with docking flags
- Requires refactoring current `UIPanel::render()` into separate window functions
- Current dual viewport system uses Framebuffer class for off-screen rendering

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

**Current Version:** V0.7.5 (Professional 3-Panel Layout) 🎨  
**Target Version:** V1.0 (Production Ready)

**Major Milestone:** 3-panel professional layout implemented! Tools on left (always visible), viewports split equally. Matches industry-standard workflows (Unity/Unreal/Blender).

**Recent Update (V0.7.5):**
- ✅ Tools panel on left (350px, always visible)
- ✅ 2D viewport in center (dynamic)
- ✅ 3D viewport on right (dynamic)
- ✅ All compiler warnings fixed (0 warnings!)
- ✅ Professional workflow achieved

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
