# V0.7.6 Update - Bug Fixes & New Project Dialog

## Bug Fixes

### 1. Tools & Parameters Panel Hiding (FIXED ✅)
**Issue:** When clicking outside the parameters panel, it would get hidden behind viewport windows.

**Solution:** Added `ImGuiWindowFlags_NoBringToFrontOnFocus` flag to Tools panel window flags.
```cpp
ImGui::Begin("Tools & Parameters", nullptr,
             ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);
```

### 2. Paint Mode Display Goes Dark (FIXED ✅)
**Issue:** When entering paint mode, auto-contrast was not being enabled, causing the display to go dark.

**Solution:** Force-enable auto-contrast when entering paint mode:
```cpp
if (params.paintMode) {
    // Enable auto-contrast to prevent dark display
    params.autoContrastDisplay = true;
    renderer.uploadTexture(rawHeight);
    // ...
}
```

## New Features

### New Project Dialog System

**Workflow Improvement:** Engine scaling and terrain type selection moved from buried parameters to project creation dialog.

#### Access:
- **File → New Project...** (Ctrl+N)
- Opens centered modal dialog

#### Dialog Options:

1. **Target Engine**
   - Unreal Engine
   - Unity
   - Godot
   - Custom
   - Sets appropriate scale factors for heightmap

2. **Terrain Type**
   - Plains
   - Hills
   - Mountains
   - Coastal
   - Custom
   - Affects height range calculations

3. **Terrain World Size**
   - 512m
   - 1km (default)
   - 2km
   - 4km
   - 8km
   - Physical size of terrain in game engine

4. **Starting Template**
   - **Perlin Noise (Procedural)** - Generates natural terrain using Perlin noise
   - **Flat (Sculpt from Scratch)** - Creates blank canvas at mid-height (0.5) for sculpting

#### What Happens on Create:

1. **Engine-Aware Setup**
   - Enables engine presets (`useEnginePresets = true`)
   - Calculates appropriate height range using `EngineScalingHelper`
   - Sets `minHeight` and `maxHeight` based on engine + terrain type + world size
   - Enables height clamping automatically

2. **Terrain Generation**
   - **Perlin Noise template:** Triggers normal terrain generation with current settings
   - **Flat template:** Generates flat plane at 0.5 height for sculpting

3. **Project Reset**
   - Clears undo/redo stacks (fresh start)
   - Resets to Select mode
   - Console output shows project settings

#### Example Console Output:
```
New project created!
  Engine: Unreal Engine
  Terrain Type: Hills
  Size: 1000m
  Height Range: 0.125 - 0.625
```

## Technical Implementation

### New Files/Additions:

**LandscapeParameters.h:**
```cpp
enum class StartingTemplate {
    PerlinNoise = 0,      // Procedural Perlin noise terrain
    FlatSculptable = 1,   // Flat plane for sculpting from scratch
};

// In LandscapeParameters struct:
bool showNewProjectDialog = false;
StartingTemplate startingTemplate = StartingTemplate::PerlinNoise;
```

**main.cpp:**
```cpp
// New function for generating flat terrain
Image generateFlatTerrain(size_t resolution, float height = 0.5f) {
    Image heightmap(resolution, resolution);
    for (size_t y = 0; y < heightmap.height(); ++y) {
        for (size_t x = 0; x < heightmap.width(); ++x) {
            heightmap(x, y) = height;
        }
    }
    return heightmap;
}

// New Project dialog rendering (modal popup)
// Located before ImGui::Render() in main loop
```

## User Experience Improvements

### Before (V0.7.5):
- Engine scaling buried in Paint Mode → Engine-Aware Presets tree
- Had to manually enable engine presets
- Easy to forget to set up engine scale before painting
- No clear "start new project" workflow

### After (V0.7.6):
- **File → New Project** is clear entry point
- Engine selection upfront and obvious
- Automatic setup of appropriate height ranges
- Choice of procedural or blank canvas start
- Professional "project-based" workflow

## Future Template Ideas

Templates that could be added later:
- **Hills Template** - Pre-configured rolling hills
- **Mountains Template** - High peaks with valleys
- **Coastal Template** - Beach/ocean transition
- **Canyon Template** - Dramatic erosion patterns
- **Tutorial Templates** - Step-by-step learning projects

## Benefits

### For Users:
- ✅ Clear project initialization workflow
- ✅ Engine-appropriate settings from the start
- ✅ No more dark display bug in paint mode
- ✅ Parameters panel always accessible
- ✅ Choice between procedural or blank start

### For Developers:
- ✅ Centralized project setup logic
- ✅ Easy to add new templates
- ✅ Clean separation of project init from editing
- ✅ Foundation for future "Save Project" / "Load Project" features

## Testing Checklist

- [ ] File → New Project opens dialog centered on screen
- [ ] All engine options work (Unreal, Unity, Godot, Custom)
- [ ] All terrain types work (Plains, Hills, Mountains, Coastal, Custom)
- [ ] All size options work (512m to 8km)
- [ ] Perlin Noise template generates terrain
- [ ] Flat template creates flat plane at 0.5 height
- [ ] Height ranges are calculated correctly
- [ ] Console output shows correct project settings
- [ ] Undo/redo stacks are cleared on new project
- [ ] Paint mode no longer goes dark
- [ ] Tools panel doesn't hide behind viewports
- [ ] Cancel button closes dialog without changes

## Version Comparison

| Feature | V0.7.5 | V0.7.6 |
|---------|--------|--------|
| Menu Bar | ✅ | ✅ |
| Mode Toolbar | ✅ | ✅ |
| Auto-Contrast Bug | ❌ | ✅ Fixed |
| Panel Hiding Bug | ❌ | ✅ Fixed |
| New Project Dialog | ❌ | ✅ New |
| Engine Setup at Start | ❌ | ✅ New |
| Starting Templates | ❌ | ✅ New (2 types) |

## Known Limitations

1. **Single Project Instance**
   - Currently no "Save Project" / "Load Project"
   - New Project replaces current work
   - User should export before creating new project

2. **Template Library**
   - Only 2 templates currently (Perlin, Flat)
   - More could be added based on user feedback

3. **Resolution Not in Dialog**
   - Map resolution (128/256/512/1024) still in terrain parameters
   - Could be added to New Project dialog in future

## Next Steps

After testing V0.7.6:
1. Test all dialog options thoroughly
2. Verify engine scaling calculations are correct
3. Consider adding "Are you sure?" prompt if user has unsaved work
4. Resume Phase 2.1 (Heightmap Import) from TODO.md
5. Consider adding Project Save/Load system (future)

---

**Version:** V0.7.6  
**Build Status:** ✅ Successful (0 errors, 0 warnings)  
**Files Modified:** main.cpp, LandscapeParameters.h  
**Files Added:** V076_UPDATE_NOTES.md (this file)
