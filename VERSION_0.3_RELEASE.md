# Landscape Designer - Version 0.3 Update

## 🎉 New Feature: Export System

### What's New
You can now save your beautiful terrains to disk in multiple formats!

### Features Added

#### 1. **PNG Export** 📸
- Grayscale export for Height, Slope, Curvature, Water masks
- Color-coded export for Biome maps (Blue water, Green grass, Gray rock)
- High-quality 8-bit/24-bit images
- Ready for game engines, 3D software, image editors

#### 2. **RAW Export** 💾
- Binary float format with header (width, height, data)
- Perfect for scientific tools or reimporting
- Preserves exact floating-point precision
- Easy to parse in Python, MATLAB, etc.

#### 3. **Smart File Naming** 🏷️
- Automatic timestamped filenames
- Includes seed number for reproducibility
- Format: `landscape_[seed]_[timestamp]_[mode].[ext]`
- Example: `landscape_12345_20240115_143022_height.png`

#### 4. **Batch Export** ⚡
- Export all 5 layers at once with one click
- Saves: Height, Slope, Curvature, Biome, Water
- Perfect for documentation or archiving variations

### UI Updates

New **"Export"** collapsible section with:
- "Export as PNG" button - Save current view
- "Export as RAW" button - Save binary data
- "Export All Layers (PNG)" - Batch export everything
- Real-time filename preview
- Status messages in console

### Files Added
- `ImageExporter.h` - Export interface
- `ImageExporter.cpp` - Export implementation
- `EXPORT_GUIDE.md` - Comprehensive documentation

### Dependencies
- **stb_image_write** (via vcpkg) - PNG writing library

### How to Use

1. **Generate your terrain** with your desired settings
2. **Select preview mode** (Height/Slope/Curvature/Biome/Water)
3. **Open "Export" section** in the UI panel
4. **Click export button** - Files saved to executable directory!

Or use **"Export All Layers"** to save everything at once.

### Example Workflow

```
1. Generate terrain with Ridged noise, seed 42
2. Adjust Water Level to 0.3
3. Select "Height" preview
4. Click "Export as PNG"
   → Saves: landscape_42_20240115_143022_height.png
5. Switch to "Biome" preview
6. Click "Export as PNG"
   → Saves: landscape_42_20240115_143022_biome.png (color-coded!)
```

### Technical Details

**PNG Format:**
- Grayscale: Single channel, 0-255 range
- Color: RGB, 3 channels, biome-specific colors
- Uses stb_image_write for cross-platform compatibility

**RAW Format:**
```
[4 bytes: width]
[4 bytes: height]
[width × height × 4 bytes: float data]
```

### Use Cases

✅ **Game Development** - Import into Unity/Unreal/Godot  
✅ **3D Modeling** - Use as displacement maps in Blender/Maya  
✅ **Image Editing** - Open in Photoshop/GIMP  
✅ **Analysis** - Process in Python/MATLAB  
✅ **Archiving** - Save favorite terrain configurations  

### Console Output

Exports show status messages:
```
Successfully exported PNG: landscape_42_20240115_143022_height.png
Successfully exported PNG: landscape_42_20240115_143022_slope.png
Successfully exported PNG: landscape_42_20240115_143022_curvature.png
...
```

---

## Previous Features (Still Included)

✅ Perlin Noise Generation (FBM, Ridged, Billowy)  
✅ Real-time Parameter Adjustment  
✅ 5 Preview Modes  
✅ Seed-based Generation  
✅ Biome Classification  
✅ Collapsible UI Sections  
✅ Statistics Display  

---

## Coming Soon

🔮 **3D Visualization** - Rotate and view terrain in 3D  
🎨 **Paint Mode** - Manually sculpt with brush tools  
🌊 **Erosion Simulation** - Hydraulic/thermal erosion  

---

**Version**: 0.3  
**Date**: 2024  
**Status**: Ready to Export! 🚀
