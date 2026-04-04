# Version 0.3 Release Notes

## Landscape Designer v0.3 - 3D Visualization Update

### Release Date
April 2026

### Major New Features

#### 🎮 3D Visualization System
- **Interactive 3D Camera**
  - Orbit/arcball camera with mouse controls
  - Left-click drag to rotate around terrain
  - Right-click drag to pan camera target
  - Mouse wheel to zoom in/out
  - Reset camera button for default view

- **3D Terrain Mesh Rendering**
  - Real-time mesh generation from heightmaps
  - Smooth vertex normals for realistic shading
  - Indexed triangle rendering for performance
  - Automatic LOD matching map resolution

- **3D Lighting System**
  - Directional lighting with ambient + diffuse components
  - Configurable light direction
  - Real-time lighting calculations in shader
  - Works with all preview modes

- **Height Scale Control**
  - Adjustable vertical exaggeration (0.1x - 2.0x)
  - Real-time mesh updates
  - Preserve horizontal scale while adjusting height

### UI Enhancements
- New "3D View" collapsible section
- Enable/disable 3D mode toggle
- Height Scale slider
- Camera info display (distance, yaw, pitch)
- Reset Camera button
- Interactive camera control instructions

### Technical Improvements
- **New Classes**
  - `Camera`: Orbit camera with view/projection matrices
  - `TerrainMesh`: 3D mesh generation and OpenGL management
  
- **Renderer Updates**
  - Dual rendering pipeline (2D quad + 3D mesh)
  - Separate shader programs for 2D and 3D
  - Depth buffer support
  - Aspect ratio handling

- **Shader System**
  - New 3D vertex shader with model/view/projection matrices
  - 3D fragment shader with lighting calculations
  - Normal transformation for proper lighting
  - Texture sampling in 3D space

### Architecture Changes
- Extended `LandscapeParameters` with `view3D` and `heightScale`
- Updated `UIPanel` to accept `Camera` pointer
- Modified main loop for mouse input handling
- Added depth testing to renderer initialization

### Dependencies
- **GLM** (OpenGL Mathematics)
  - Matrix operations (glm::mat4)
  - Vector math (glm::vec3)
  - Camera transformations (lookAt, perspective)
  - Value pointers for uniform uploads

### Performance Considerations
- Mesh generation is deferred until 3D mode enabled
- Mesh updates only on parameter changes
- Efficient indexed rendering (GL_TRIANGLES with EBO)
- Minimal CPU overhead for camera rotations

### Known Limitations
- Single mesh (no terrain chunking)
- No frustum culling
- No dynamic LOD
- Fixed lighting direction
- No shadows or advanced lighting effects

### Backward Compatibility
- All existing features preserved
- 2D view remains default
- Export system works in both 2D and 3D modes
- No changes to file formats
- Existing parameters unchanged

### Upgrade Path
1. New files added: `Camera.h/cpp`, `TerrainMesh.h/cpp`
2. Modified files: `Renderer.h/cpp`, `UIPanel.h/cpp`, `main.cpp`, `LandscapeParameters.h`
3. Updated project file: `HeightMapGenerator.vcxproj`
4. New dependency: GLM (header-only, likely already available via vcpkg)

### Usage Example
```cpp
// Enable 3D view
params.view3D = true;
params.heightScale = 0.5f;

// Camera is automatically created in main()
Camera camera;

// Mouse controls work automatically
// - Left drag rotates
// - Right drag pans
// - Scroll wheel zooms

// Reset camera to default
camera.reset();
```

### What's Next?
- **Priority 4**: Paint Mode - Manual terrain sculpting with brushes
- **Priority 5**: Erosion Simulation - Hydraulic and thermal erosion

### Contributors
Built with GitHub Copilot for Visual Studio 2026

### Documentation
- See `3D_VISUALIZATION_GUIDE.md` for detailed usage instructions
- See `README.md` for general project information
- See `EXPORT_GUIDE.md` for export functionality

---

**Version History**
- v0.1: Initial terrain generation pipeline
- v0.2: Perlin noise system + Export functionality
- v0.3: **3D Visualization** (current)
