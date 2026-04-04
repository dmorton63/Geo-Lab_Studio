# 3D Visualization Guide

## Landscape Designer v0.3 - 3D View Feature

### Overview
The 3D visualization system lets you view and interact with your terrain in full 3D with camera controls, lighting, and mesh rendering.

### Enabling 3D View
1. Open the "3D View" section in the UI
2. Check "Enable 3D View"
3. The terrain will automatically convert to a 3D mesh

### Camera Controls
- **Rotate**: Left click + drag to orbit around the terrain
- **Pan**: Right click + drag to move the camera target
- **Zoom**: Mouse wheel to move closer or farther

### 3D View Parameters
- **Height Scale** (0.1 - 2.0): Controls the vertical exaggeration of the terrain
  - Lower values (0.1-0.5): Subtle elevation changes
  - Higher values (1.0-2.0): Dramatic, exaggerated terrain

### Camera Information
The UI displays current camera state:
- **Distance**: How far the camera is from the terrain center
- **Yaw**: Horizontal rotation angle (0-360°)
- **Pitch**: Vertical angle (-89° to 89°)

### Reset Camera
Click "Reset Camera" to return to the default view:
- Distance: 3.0 units
- Yaw: 0°
- Pitch: 45°

### Preview Modes in 3D
All preview modes work in 3D view:
- **Height**: Grayscale elevation with lighting
- **Slope**: Color-coded slope angle with lighting
- **Curvature**: Color-coded surface curvature
- **Biome**: Color-coded terrain types (water/grass/rock)
- **Water Mask**: Water areas highlighted

### 3D Rendering Features
- **Real-time lighting**: Directional light from upper-right
- **Smooth normals**: Calculated per-vertex for smooth shading
- **Ambient + Diffuse**: Combination of ambient (30%) and diffuse (70%) lighting
- **Automatic aspect ratio**: Adjusts to window size

### Performance Tips
- Lower map resolution (128x128) for smoother interaction
- Disable auto-update when adjusting camera
- Use Height Scale around 0.5 for most terrains

### Exporting 3D Terrain
While in 3D view, you can still export:
- Heightmaps work the same in 2D and 3D modes
- RAW files can be imported into 3D software
- PNG files provide visual references

### Workflow Recommendations
1. Generate terrain in 2D view with desired parameters
2. Switch to 3D view to inspect the result
3. Adjust Height Scale to see details
4. Rotate to check for artifacts or interesting features
5. Return to 2D to tweak parameters if needed
6. Export when satisfied

### Technical Details
- **Mesh Resolution**: Matches map resolution (e.g., 256x256 = 65,536 vertices)
- **Render Mode**: Triangle mesh with indexed drawing
- **Lighting Model**: Phong-style with ambient + diffuse components
- **Coordinate Space**: Normalized [-1, 1] XZ plane with Y as height

### Troubleshooting
- **Terrain appears flat**: Increase Height Scale
- **Performance issues**: Lower map resolution
- **Camera stuck**: Use Reset Camera button
- **Missing terrain**: Generate or update terrain first

Enjoy exploring your procedural landscapes in 3D!
