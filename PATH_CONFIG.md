# **TODO.md — Terrain Editor & Engine‑Aware Scaling System**  
### *Development Roadmap & Implementation Checklist*

---

## **1. Core Architecture**
### **1.1 Multi‑View Editing System**
- [ ] Maintain **Heightmap Model** as single source of truth  
- [ ] Implement **Editor View (2D Orthographic)** for direct manipulation  
- [ ] Implement **3D Viewer** as lightweight renderer  
- [ ] Create **event‑driven update pipeline**  
  - [ ] Editor modifies heightmap  
  - [ ] Heightmap broadcasts update event  
  - [ ] 3D viewer regenerates mesh  
  - [ ] All views sync in real time  

### **1.2 Data Flow**
- [ ] Centralized heightmap storage  
- [ ] Efficient mesh regeneration  
- [ ] Partial update support (future)  
- [ ] Undo/redo stack for all operations  

---

## **2. Heightmap Import Pipeline**
### **2.1 Basic Import**
- [ ] Load RAW/PNG/EXR heightmaps  
- [ ] Normalize grayscale to internal height range  
- [ ] Detect min/max elevation  

### **2.2 Engine‑Aware Import Mode**
- [ ] Add **Target Engine Dropdown**  
  - [ ] Unreal Engine  
  - [ ] Unity  
  - [ ] Godot  
  - [ ] CryEngine  
  - [ ] Custom Engine (manual input)  
- [ ] Load engine defaults:  
  - [ ] Character height  
  - [ ] Capsule radius  
  - [ ] Walkable slope angle  
  - [ ] World unit scale (cm/m)  
  - [ ] Recommended terrain height ranges  

### **2.3 Character‑Relative Scaling**
- [ ] Apply baseline scaling using engine’s default character size  
- [ ] Ensure hills/mountains feel correct at human scale  
- [ ] Clamp extreme slopes to walkable limits (optional)  

---

## **3. Terrain Semantic Analysis**
### **3.1 Feature Detection**
- [ ] Compute slope map  
- [ ] Compute curvature map  
- [ ] Detect local maxima/minima  
- [ ] Watershed detection for rivers/valleys  

### **3.2 Classification**
- [ ] Plains  
- [ ] Hills  
- [ ] Mountains  
- [ ] Rivers / Valleys  
- [ ] Basins / Depressions  
- [ ] Ridges / Peaks  

### **3.3 Subdivision**
- [ ] Small  
- [ ] Medium  
- [ ] Large  
- [ ] Mega (extreme outliers)  

---

## **4. Smart Scaling System**
### **4.1 Category‑Based Scaling**
- [ ] UI panel listing all terrain categories  
- [ ] Min/Max multiplier fields per category  
- [ ] Non‑uniform scaling applied per classification  
- [ ] Preserve relative detail within each category  

### **4.2 Real‑Time Preview Slider**
- [ ] Global slider interpolates between min/max multipliers  
- [ ] Heightmap updates in real time  
- [ ] 3D view updates instantly  
- [ ] Editor view updates instantly  
- [ ] Slope/curvature maps update accordingly  

### **4.3 Developer Controls**
- [ ] Color‑intensity‑based scaling option  
- [ ] Custom multiplier curves (future)  
- [ ] Presets for terrain types (future)  

---

## **5. Editing Tools**
### **5.1 Brushes**
- [ ] Raise/Lower  
- [ ] Smooth  
- [ ] Flatten  
- [ ] Noise  
- [ ] Blend  

### **5.2 Stamps**
- [ ] Stamp placement system  
- [ ] Stamp rotation/scale  
- [ ] Stamp blending modes  
- [ ] Stamp library support  

### **5.3 Region Masks**
- [ ] Box/lasso selection  
- [ ] Apply operations to masked areas  
- [ ] Save/load masks  

### **5.4 Splines (Future)**
- [ ] Catmull‑Rom or Kochanek–Bartels splines  
- [ ] Terrain deformation along spline  
- [ ] Road/river tools  
- [ ] Spline‑based stamp placement  

---

## **6. Object Placement (Future Milestone)**
### **6.1 FBX Import**
- [ ] Load FBX meshes  
- [ ] Upload to GPU  
- [ ] Render in 3D view  

### **6.2 Placement Tools**
- [ ] Translate/rotate/scale  
- [ ] Snap to terrain  
- [ ] Scatter tools (future)  

### **6.3 Export**
- [ ] Export object transforms  
- [ ] Export terrain + objects to UE/Unity/Godot  

---

## **7. Export Pipeline**
### **7.1 Heightmap Export**
- [ ] Export normalized heightmap  
- [ ] Apply engine‑specific scaling  
- [ ] Ensure correct world‑unit conversion  

### **7.2 Metadata Export (Optional)**
- [ ] Slope map  
- [ ] Curvature map  
- [ ] Biome masks  
- [ ] Region classifications  

### **7.3 Engine‑Specific Presets**
- [ ] UE Landscape  
- [ ] Unity Terrain  
- [ ] Godot HeightMapShape  
- [ ] Custom engine template  

---

## **8. Future Enhancements**
- [ ] Biome‑aware scaling  
- [ ] Walkability heatmaps  
- [ ] Player‑height preview camera  
- [ ] Erosion simulation events  
- [ ] World event system (tectonics, flooding, glacial carving)  
- [ ] World Composer mode (multi‑layer editing)  

