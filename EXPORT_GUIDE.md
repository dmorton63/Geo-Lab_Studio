# Export System Guide

## Overview
The Landscape Designer now supports exporting your generated terrains in multiple formats!

## Export Formats

### PNG Format
- **Grayscale Images**: Height, Slope, Curvature, Water Mask
  - 8-bit grayscale (0-255)
  - Normalized to visible range
  - Compatible with image editors and game engines

- **Color-Coded Images**: Biome Mask
  - 24-bit RGB color
  - Water: Blue (50, 100, 200)
  - Grass: Green (50, 180, 50)
  - Rock: Gray (150, 150, 150)

### RAW Format
- Binary float data with header
- Header: width (4 bytes) + height (4 bytes)
- Data: Float array (width × height × 4 bytes)
- Perfect for reimporting or use in scientific tools

## How to Export

### Quick Export (Current View)
1. Generate your terrain
2. Select the preview mode you want to export (Height/Slope/Curvature/Biome/Water)
3. Click **"Export as PNG"** or **"Export as RAW"**
4. File saved automatically with timestamp

### Batch Export (All Layers)
- Click **"Export All Layers (PNG)"**
- Exports all 5 preview modes at once:
  - `landscape_[seed]_[timestamp]_height.png`
  - `landscape_[seed]_[timestamp]_slope.png`
  - `landscape_[seed]_[timestamp]_curvature.png`
  - `landscape_[seed]_[timestamp]_biome.png`
  - `landscape_[seed]_[timestamp]_water.png`

## File Naming Convention
Format: `landscape_[seed]_[timestamp]_[mode].[ext]`

Example: `landscape_42_20240115_143022_height.png`
- Seed: 42
- Date: 2024-01-15
- Time: 14:30:22
- Mode: height
- Format: PNG

## Export Location
Files are saved to the same directory as the executable (typically the project's build output folder).

## Use Cases

### Game Development
- Import heightmaps into Unity, Unreal, or Godot
- Use biome maps for texture splatting
- Use water masks for shader effects

### 3D Modeling
- Import RAW heightmaps into Blender/Maya
- Use as displacement maps
- Generate terrain meshes

### Further Processing
- Load into Photoshop/GIMP for manual editing
- Use slope maps for erosion simulation input
- Combine multiple exports for complex effects

## Tips
- **Higher Resolution**: Use 512 or 1024 for detailed exports (takes longer to generate)
- **Biome Export**: Always export as PNG to preserve colors
- **RAW Format**: Best for numerical analysis or reimporting exact data
- **Batch Export**: Great for documenting different terrain variations

## Technical Details

### PNG Specifications
- Bit depth: 8-bit grayscale or 24-bit RGB
- No compression quality loss
- Standard PNG format (compatible everywhere)

### RAW Specifications
```
Byte Layout:
0-3:   Width (uint32_t, little-endian)
4-7:   Height (uint32_t, little-endian)
8-end: Float data (width × height floats)
```

### To Read RAW in Python:
```python
import struct
import numpy as np

with open('landscape_height.raw', 'rb') as f:
    width, height = struct.unpack('II', f.read(8))
    data = np.fromfile(f, dtype=np.float32)
    heightmap = data.reshape((height, width))
```

---

**Version**: 0.3  
**Feature Added**: Export System  
**Dependencies**: stb_image_write
