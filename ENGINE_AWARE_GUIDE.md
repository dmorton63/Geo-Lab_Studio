# Engine-Aware Height Presets - Implementation Guide
## Character-Relative Terrain Scaling System

### **The Problem**
When creating heightmaps for games, it's hard to know what "natural" height ranges look like:
- Is 0.5 in the heightmap a hill or a mountain?
- Will my character be able to climb this slope?
- Does this terrain feel proportional to the player?

**Solution:** Use real-world character dimensions to establish natural-looking height ranges!

---

### **How It Works**

#### 1. **Select Your Engine**
Each engine has default character dimensions:

| Engine | Character Height | Capsule Radius | Walk Angle |
|--------|-----------------|----------------|------------|
| **Unreal Engine 5** | 180cm | 42cm | 45° |
| **Unity** | 200cm | 50cm | 45° |
| **Godot 4** | 180cm | 40cm | 46° |
| **Custom** | User-defined | - | - |

#### 2. **Choose Terrain Type**
The system calculates natural height ranges relative to character height:

##### **🏞️ Plains** (Subtle, Walkable)
- **Height Range**: 0m to 1.2× character height
- **Example (180cm char)**: 0m to 2.1m variation
- **Use For**: Farmlands, grasslands, gentle rolling hills
- **Gameplay**: Fully traversable, no obstacles

##### **⛰️ Hills** (Dramatic but Traversable)
- **Height Range**: 0m to 8× character height
- **Example (180cm char)**: 0m to 14.4m variation
- **Use For**: Hilly terrain, moderate elevation changes
- **Gameplay**: Climbable but noticeable effort

##### **🏔️ Mountains** (Epic Peaks)
- **Height Range**: 0m to 55× character height
- **Example (180cm char)**: 0m to 99m variation
- **Use For**: Mountain ranges, dramatic vistas
- **Gameplay**: Requires paths, climbing systems, or vehicles

##### **🏖️ Coastal** (Beach + Ocean)
- **Height Range**: -2× to 3× character height
- **Example (180cm char)**: -3.6m to 5.4m
- **Use For**: Beaches, ocean floors, coastal cliffs
- **Gameplay**: Swimming/diving areas + beach walks

---

### **Math Behind the System**

#### **Terrain World Size**
The physical size of your terrain in the engine (e.g., 1000m × 1000m).

#### **Normalization Formula**
```cpp
normalized_height = real_world_meters / terrain_world_size

Example:
- Terrain: 1000m × 1000m
- Mountain peak: 100m tall
- Normalized: 100 / 1000 = 0.10 (10% of terrain size)
```

#### **Why This Works**
- **Proportional Scaling**: Heights scale with terrain size
- **Engine-Agnostic**: Works with any engine's coordinate system
- **Character-Relative**: Always looks natural for your player scale

---

### **User Interface**

#### **Location**
Paint Mode → "Engine-Aware Presets" (collapsible tree node)

#### **Controls**

1. **Enable Checkbox**
   - Toggles engine-aware mode
   - Automatically enables height clamping
   - Calculates initial range based on current settings

2. **Target Engine Dropdown**
   - Unreal Engine, Unity, Godot, Custom
   - Auto-loads character height defaults
   - Recalculates ranges on change

3. **Terrain Type Dropdown**
   - Plains, Hills, Mountains, Coastal, Custom
   - Each has predefined height multipliers
   - Recalculates ranges on change

4. **Character Height Slider** (50cm - 300cm)
   - Fine-tune for your specific character
   - Defaults loaded from engine preset
   - Live updates range as you adjust

5. **Terrain Size Slider** (100m - 5000m)
   - Set your target terrain's physical size in engine
   - Default: 1000m (common for game terrains)
   - Affects normalization calculation

6. **Info Display**
   - Shows current engine specs
   - Displays height range in both:
     - **Normalized** (0.0 - 1.0 for heightmap)
     - **Real-world** (meters, for context)
   - Includes terrain type description

7. **Recalculate Button**
   - Manually refreshes range if needed
   - Useful after manual tweaks

---

### **Example Workflows**

#### **Scenario 1: Unreal Engine Mountain Range**
1. Enable Paint Mode
2. Open "Engine-Aware Presets"
3. Check "Use Engine Presets"
4. Select:
   - Engine: **Unreal Engine**
   - Terrain Type: **Mountains**
   - Terrain Size: **2000m**
5. Result:
   - Character: 180cm (1.8m)
   - Range: 0m to 99m
   - Normalized: 0.000 to 0.050
   - Painting automatically clamped to dramatic but realistic peaks!

#### **Scenario 2: Unity Coastal Scene**
1. Enable Paint Mode
2. Open "Engine-Aware Presets"
3. Check "Use Engine Presets"
4. Select:
   - Engine: **Unity**
   - Terrain Type: **Coastal**
   - Terrain Size: **500m**
5. Result:
   - Character: 200cm (2.0m)
   - Range: -4m to 6m
   - Normalized: -0.008 to 0.012
   - Ocean floor + beach + gentle cliffs perfectly proportioned!

#### **Scenario 3: Custom Indie Game**
1. Enable Paint Mode
2. Open "Engine-Aware Presets"
3. Check "Use Engine Presets"
4. Select:
   - Engine: **Custom**
   - Terrain Type: **Hills**
   - Character Height: **120cm** (child protagonist)
   - Terrain Size: **400m**
5. Result:
   - Range: 0m to 9.6m
   - Hills feel larger relative to smaller character!

---

### **Integration with Existing Features**

#### **Works With**
- ✅ **Height Clamping**: Auto-enables and sets min/max
- ✅ **Soft/Hard Clamp Modes**: Preserves user preference
- ✅ **All Brushes**: Raise, Lower, Smooth, Flatten
- ✅ **Status Overlay**: Shows real-time height relative to limits
- ✅ **Undo/Redo**: Full support
- ✅ **Noise Generation**: Generate directly into natural ranges

#### **Best Practices**
1. **Set engine presets BEFORE painting**
   - Ensures consistent height scaling from start
2. **Use soft clamp mode**
   - More natural for organic terrain
3. **Adjust terrain size to match your engine project**
   - Unity Terrain: Check Inspector size
   - UE Landscape: Check Section Size × Components
4. **Test in engine early**
   - Export RAW, import, verify scale feels right
5. **Save character height for your project**
   - Consistency across all heightmaps

---

### **Technical Implementation**

#### **Files Modified**
1. `LandscapeParameters.h`
   - Added `TargetEngine` enum
   - Added `TerrainType` enum
   - Added engine-aware parameters

2. `EngineScalingHelper.h` (NEW)
   - Engine specifications database
   - Terrain range calculations
   - Normalization formulas
   - Description generation

3. `UIPanel.cpp`
   - Engine-Aware Presets UI section
   - Live range calculation
   - Info display

#### **Key Formulas**
```cpp
// Terrain range based on character height
TerrainRange getTerrainRange(TerrainType type, float charHeightCm) {
    float charHeightM = charHeightCm / 100.0f;
    
    switch (type) {
        case Plains:   return { 0.0f, charHeightM * 1.2f };
        case Hills:    return { 0.0f, charHeightM * 8.0f };
        case Mountains: return { 0.0f, charHeightM * 55.0f };
        case Coastal:  return { -charHeightM * 2.0f, charHeightM * 3.0f };
    }
}

// Convert meters to normalized (0.0-1.0)
float metersToNormalized(float meters, float terrainWorldSize) {
    return meters / terrainWorldSize;
}

// Calculate final clamp range
std::pair<float, float> calculateClampRange(params) {
    TerrainRange range = getTerrainRange(params.terrainType, params.characterHeight);
    
    float minNorm = metersToNormalized(range.min, params.terrainWorldSize);
    float maxNorm = metersToNormalized(range.max, params.terrainWorldSize);
    
    return { clamp(minNorm, 0, 1), clamp(maxNorm, 0, 1) };
}
```

---

### **Future Enhancements**

#### **Planned (Phase 5)**
- [ ] Walkability heatmap based on slope vs walkable angle
- [ ] Preview showing character-relative scale markers
- [ ] "Place character model" to visualize scale in 3D
- [ ] Save engine presets per project
- [ ] Import engine project settings automatically

#### **Potential**
- [ ] Multi-terrain blending (coast → plains → hills → mountains)
- [ ] Dynamic range adjustment based on painted features
- [ ] AI-assisted terrain type detection
- [ ] Export with scale metadata for engine import scripts

---

### **FAQ**

**Q: Will this break my existing heightmaps?**  
A: No! It only affects NEW painting when enabled. Existing data unchanged.

**Q: Can I use custom character sizes?**  
A: Yes! Select "Custom" engine or adjust the slider after choosing a preset.

**Q: What if my terrain is 4000m × 4000m?**  
A: Adjust "Terrain Size" slider. System recalculates automatically.

**Q: Do I need to know my engine's exact specs?**  
A: No! We provide defaults. But you can customize if needed.

**Q: Can I still use manual min/max sliders?**  
A: Yes! Turn off "Use Engine Presets" to use manual mode.

**Q: What about vertical games (skyscrapers, caves)?**  
A: Use Custom terrain type and set your own height multipliers.

---

### **Real-World Scale Reference**

For context when setting character height:

| Character Type | Height | Example |
|---------------|--------|---------|
| Child | 100-140cm | Kid protagonist |
| Average Adult | 160-190cm | Third-person games |
| Tall Character | 190-220cm | Action heroes |
| Giant/Mech | 300-500cm | Boss characters |
| Vehicle | Varies | Driving games |

**Standard Terrain Sizes:**
- **Small**: 500m × 500m (tight arena)
- **Medium**: 1000m × 1000m (typical open world chunk)
- **Large**: 2000m × 2000m (expansive landscapes)
- **Massive**: 4000m+ (MMORPG zones)

---

🎉 **Now your terrain will always look natural in your game engine!**
