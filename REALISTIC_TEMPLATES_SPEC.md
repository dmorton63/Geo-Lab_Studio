# Realistic Engine-Specific Templates - Design Specification

## 📋 Overview

**Feature:** Pre-configured, professional-quality terrain templates optimized for specific game engines and terrain types.

**Problem Solved:** Users currently start with either generic Perlin noise or flat terrain. Neither provides realistic, engine-appropriate starting points. This leads to:
- Time wasted tweaking parameters
- Inconsistent results
- Terrain that doesn't match engine scale
- Frustration for beginners

**Solution:** Curated library of realistic templates that instantly generate professional terrain perfectly scaled for the target engine.

---

## 🎯 Goals

1. **Professional Quality** - Every template produces game-ready terrain
2. **Engine-Optimized** - Heights perfect for each engine's character scale
3. **Time-Saving** - Instant realistic terrain, no parameter tweaking
4. **Educational** - Users can study good terrain parameters
5. **Consistent** - Every project starts on solid foundation

---

## 📊 Template Categories

### 1. Hills Templates
**Use Case:** Rolling countryside, gentle open-world terrain, farmland

**Characteristics:**
- Gentle slopes (walkable)
- Rounded peaks
- Smooth valleys
- Good for exploration gameplay

**Engine Variants:**

#### Unreal Hills
- **Height Range:** 0-20m
- **Noise Type:** FBM (Fractal Brownian Motion)
- **Octaves:** 4
- **Persistence:** 0.5
- **Lacunarity:** 2.0
- **Scale:** 3.0
- **Erosion:** Light thermal (talus angle: 0.05, 5 iterations)
- **World Size:** 2km recommended
- **Character Scale:** Perfect for 180cm character
- **Description:** "Rolling hills perfect for UE5 landscapes. Gentle slopes ideal for exploration and open-world games."

#### Unity Hills
- **Height Range:** 0-15m
- **Noise Type:** FBM
- **Octaves:** 3
- **Persistence:** 0.45
- **Lacunarity:** 2.2
- **Scale:** 2.5
- **Erosion:** Minimal
- **World Size:** 1km recommended
- **Character Scale:** Unity default (1.8m)
- **Description:** "Smooth rolling terrain optimized for Unity Terrain system. Great for painting biomes and placement."

#### Godot Hills
- **Height Range:** 0-18m
- **Noise Type:** FBM
- **Octaves:** 3
- **Persistence:** 0.5
- **Lacunarity:** 2.0
- **Scale:** 2.8
- **Erosion:** Light
- **World Size:** 1km recommended
- **Character Scale:** Standard (1.8m)
- **Description:** "Balanced terrain for Godot's GridMap system. Good detail vs performance balance."

---

### 2. Mountains Templates
**Use Case:** Dramatic landscapes, alpine environments, climbing gameplay

**Characteristics:**
- Sharp peaks
- Deep valleys
- Steep slopes (cliffs)
- High elevation variation
- Realistic erosion patterns

**Engine Variants:**

#### Unreal Mountains
- **Height Range:** 0-150m
- **Noise Type:** Ridged
- **Octaves:** 6
- **Persistence:** 0.6
- **Lacunarity:** 2.2
- **Scale:** 4.5
- **Erosion:** 
  - Hydraulic: 50,000 droplets, rain 2.0
  - Thermal: Talus 0.08, 10 iterations
- **World Size:** 4km recommended
- **Character Scale:** 180cm character
- **Features:**
  - Water channels from hydraulic erosion
  - Rock slides from thermal erosion
  - Dramatic peaks and valleys
- **Description:** "Dramatic mountain terrain for UE5. Pre-eroded for realistic water channels and rock formations."

#### Unity Mountains
- **Height Range:** 0-120m
- **Noise Type:** Ridged
- **Octaves:** 5
- **Persistence:** 0.55
- **Lacunarity:** 2.1
- **Scale:** 4.0
- **Erosion:** Moderate hydraulic + thermal
- **World Size:** 2km recommended
- **Description:** "High-detail alpine terrain for Unity. Multiple noise octaves create realistic mountain detail."

#### Godot Mountains
- **Height Range:** 0-100m
- **Noise Type:** Ridged
- **Octaves:** 4
- **Persistence:** 0.5
- **Lacunarity:** 2.0
- **Scale:** 3.5
- **Erosion:** Light (performance-optimized)
- **World Size:** 2km recommended
- **Description:** "Performance-optimized mountain terrain for Godot. Good detail without excessive complexity."

---

### 3. Coastal Templates
**Use Case:** Beach levels, island exploration, naval gameplay

**Characteristics:**
- Underwater terrain (negative elevation)
- Sandy beaches (gentle slopes near water)
- Coastal cliffs
- Water masking pre-configured
- Smooth transitions water → land

**Engine Variants:**

#### Unreal Coastal
- **Height Range:** -20m to +30m
- **Noise Type:** FBM + Ridged blend
- **Features:**
  - Underwater ocean floor (-20m to 0m)
  - Sandy beach transition (0m to +2m)
  - Coastal cliffs (+5m to +30m)
  - Pre-masked water areas (below 0m)
- **World Size:** 2km recommended
- **Description:** "Complete coastal environment for UE5. Underwater terrain, beaches, and cliffs. Water mask included."

#### Unity Coastal
- **Height Range:** -15m to +25m
- **Noise Type:** FBM
- **Features:**
  - Gentle underwater slopes
  - Gradual beach transitions
  - Rocky coastal features
- **World Size:** 1km recommended
- **Description:** "Island shoreline terrain for Unity. Smooth beaches with gradual slopes, perfect for Unity Water System."

#### Godot Coastal
- **Height Range:** -10m to +20m
- **Noise Type:** FBM (simplified)
- **Features:**
  - Small island terrain
  - Simple underwater
  - Beach areas
- **World Size:** 1km recommended
- **Description:** "Small island terrain for Godot. Optimized for performance with good visual quality."

---

### 4. Plains Templates
**Use Case:** Open-world exploration, farmland, vast landscapes

**Characteristics:**
- Minimal elevation change
- Very gentle slopes
- Subtle micro-variations
- Good for large-scale worlds
- Perfect for building/placement

**Engine Variants:**

#### Unreal Plains
- **Height Range:** 0-5m
- **Noise Type:** Classic (very low frequency)
- **Octaves:** 2
- **Scale:** 1.5
- **Features:**
  - Subtle undulation
  - Perfect for grass rendering
  - Good for building placement
- **World Size:** 4km+ recommended
- **Description:** "Vast open grasslands for UE5. Subtle elevation changes perfect for large open-world games."

#### Unity Plains
- **Height Range:** 0-3m
- **Noise Type:** Classic
- **Octaves:** 2
- **Scale:** 1.2
- **Features:**
  - Minimal variation
  - Farmland-style terrain
  - Excellent for Detail Painting
- **World Size:** 2km recommended
- **Description:** "Farmland/prairie terrain for Unity. Minimal elevation change, perfect for grass and detail painting."

#### Godot Plains
- **Height Range:** 0-4m
- **Noise Type:** Classic
- **Octaves:** 2
- **Scale:** 1.3
- **Features:**
  - Flat with micro-details
  - Good for GridMap
  - Low poly-friendly
- **World Size:** 2km recommended
- **Description:** "Flat terrain with subtle detail for Godot. Optimized for large open areas."

---

### 5. Desert Templates
**Use Case:** Arid environments, dune exploration, desert combat

**Characteristics:**
- Rolling dunes (billowy noise)
- Smooth wind-swept appearance
- Rock formations (ridged noise blend)
- Minimal erosion (wind, not water)
- Sandy textures suggested

**Engine Variants:**

#### Unreal Desert
- **Height Range:** 0-40m
- **Noise Type:** Billowy (70%) + Ridged (30%) blend
- **Features:**
  - Large rolling dunes (billowy)
  - Rocky outcrops (ridged)
  - Wind-swept appearance (smooth erosion)
- **Erosion:** Custom smooth erosion (simulates wind)
- **World Size:** 4km recommended
- **Description:** "Sand dunes and rock formations for UE5. Billowy noise creates realistic dune shapes."

#### Unity Desert
- **Height Range:** 0-30m
- **Noise Type:** Billowy
- **Features:**
  - Rolling dunes
  - Smooth transitions
  - Good for Unity Terrain
- **World Size:** 2km recommended
- **Description:** "Rolling sand dunes for Unity. Smooth billowy noise perfect for desert environments."

#### Godot Desert
- **Height Range:** 0-25m
- **Noise Type:** Billowy (simplified)
- **Features:**
  - Simple dune terrain
  - Performance-optimized
- **World Size:** 2km recommended
- **Description:** "Simple dune terrain for Godot. Good balance of detail and performance."

---

### 6. Volcanic Templates
**Use Case:** Lava levels, volcanic islands, dramatic landscapes

**Characteristics:**
- Central peak/caldera
- Sharp ridges (ridged noise)
- Flat crater floors
- Steep outer slopes
- Potential lava flow channels

**Engine Variants:**

#### Unreal Volcanic
- **Height Range:** 0-200m
- **Noise Type:** Ridged + Custom crater mask
- **Features:**
  - Central caldera (flat at top)
  - Sharp ridge peaks
  - Steep outer slopes
  - Potential lava channel masks
- **World Size:** 2km recommended
- **Description:** "Volcanic caldera and lava flows for UE5. Sharp ridged peaks with flat crater floor."

#### Unity Volcanic
- **Height Range:** 0-150m
- **Noise Type:** Ridged
- **Features:**
  - Mountain cone with crater
  - Sharp features
  - Good for Unity's terrain
- **World Size:** 1km recommended
- **Description:** "Volcanic mountain with crater for Unity. Dramatic ridged terrain."

#### Godot Volcanic
- **Height Range:** 0-120m
- **Noise Type:** Ridged (simplified)
- **Features:**
  - Simple volcanic cone
  - Performance-optimized
- **World Size:** 1km recommended
- **Description:** "Simple volcanic cone for Godot. Optimized ridged terrain."

---

## 🏗️ Implementation Architecture

### Template Data Structure

```cpp
struct TerrainTemplate {
    std::string name;               // "Unreal Mountains"
    std::string description;        // "Dramatic mountain terrain..."
    TargetEngine engine;            // Unreal, Unity, Godot
    TerrainCategory category;       // Hills, Mountains, Coastal, etc.
    
    // Noise parameters
    NoiseType noiseType;            // FBM, Ridged, Billowy
    int octaves;
    float persistence;
    float lacunarity;
    float scale;
    unsigned int seed;              // Default seed (user can change)
    
    // Height range
    float minHeight;                // e.g., -20m for coastal
    float maxHeight;                // e.g., +30m for coastal
    
    // Erosion settings (optional)
    bool applyHydraulicErosion;
    int hydraulicIterations;
    float rainAmount;
    float evaporation;
    float hydraulicStrength;
    
    bool applyThermalErosion;
    int thermalIterations;
    float talusAngle;
    float thermalStrength;
    
    // Recommended settings
    int recommendedResolution;      // 512 or 1024
    float recommendedWorldSize;     // 2000.0 (2km)
    
    // Additional features
    bool enableWaterMask;           // For coastal
    float waterLevel;               // Sea level for coastal
    
    // Preview thumbnail (future)
    std::string thumbnailPath;
};
```

### Template Library

```cpp
class TemplateLibrary {
public:
    static std::vector<TerrainTemplate> getAllTemplates();
    static std::vector<TerrainTemplate> getTemplatesForEngine(TargetEngine engine);
    static std::vector<TerrainTemplate> getTemplatesByCategory(TerrainCategory category);
    static TerrainTemplate getTemplate(const std::string& name);
    
    // Built-in templates
    static TerrainTemplate getUnrealHills();
    static TerrainTemplate getUnrealMountains();
    static TerrainTemplate getUnrealCoastal();
    static TerrainTemplate getUnrealPlains();
    static TerrainTemplate getUnrealDesert();
    static TerrainTemplate getUnrealVolcanic();
    
    static TerrainTemplate getUnityHills();
    // ... etc for Unity and Godot
};
```

### Template Generator

```cpp
class TemplateGenerator {
public:
    static Image generateFromTemplate(const TerrainTemplate& templ, int resolution);
    
private:
    static Image applyNoisePattern(const TerrainTemplate& templ, int resolution);
    static Image applyErosion(const Image& heightmap, const TerrainTemplate& templ);
    static Image applySpecialFeatures(const Image& heightmap, const TerrainTemplate& templ);
};
```

---

## 🎨 UI Integration

### New Project Dialog Enhancement

**Current Flow:**
1. Select Engine (Unreal/Unity/Godot)
2. Select Terrain Type (Plains/Hills/Mountains/Coastal)
3. Select World Size
4. Select Template (Perlin Noise / Flat)

**New Flow:**
1. Select Engine (Unreal/Unity/Godot)
2. **Select Template Category** (dropdown):
   - Procedural (Perlin Noise, Flat) - **current system**
   - Hills (Realistic rolling hills)
   - Mountains (Dramatic peaks and valleys)
   - Coastal (Beach, cliffs, underwater)
   - Plains (Vast open terrain)
   - Desert (Sand dunes)
   - Volcanic (Caldera and peaks)
3. **Template Preview** (shows selected template info):
   - Name: "Unreal Mountains"
   - Description: "Dramatic mountain terrain for UE5..."
   - Height Range: 0-150m
   - Erosion: Hydraulic + Thermal
   - Recommended World Size: 4km
   - Recommended Resolution: 1024
4. **"Use Recommended Settings" checkbox** (auto-sets resolution/world size)
5. Click "Create"

### Template Selector Mockup

```
┌─────────────────── New Project ───────────────────┐
│                                                    │
│ Target Engine:  [Unreal Engine ▼]                 │
│                                                    │
│ Template Category:  [Mountains ▼]                 │
│   - Procedural (Perlin Noise, Flat)              │
│   - Hills                                         │
│   - Mountains ◄── selected                        │
│   - Coastal                                       │
│   - Plains                                        │
│   - Desert                                        │
│   - Volcanic                                      │
│                                                    │
│ ┌─────── Template: Unreal Mountains ──────────┐  │
│ │ Dramatic mountain terrain for UE5.          │  │
│ │ Pre-eroded with realistic water channels    │  │
│ │ and rock formations.                        │  │
│ │                                             │  │
│ │ • Height Range: 0-150m                      │  │
│ │ • Erosion: Hydraulic + Thermal              │  │
│ │ • Recommended World Size: 4km               │  │
│ │ • Recommended Resolution: 1024              │  │
│ │                                             │  │
│ │ [Thumbnail Preview - future]                │  │
│ └─────────────────────────────────────────────┘  │
│                                                    │
│ [✓] Use Recommended Settings                      │
│                                                    │
│ Resolution: [1024 ▼]  (auto-set if ✓ above)      │
│ World Size: [4000m]   (auto-set if ✓ above)      │
│                                                    │
│              [Cancel]  [Create Project]            │
└────────────────────────────────────────────────────┘
```

---

## 📈 Benefits Analysis

### Time Savings
- **Before:** 15-30 minutes tweaking parameters to get realistic terrain
- **After:** Instant professional terrain (0 minutes)
- **Time Saved:** ~20 minutes per project
- **For 10 projects:** 200 minutes saved = **3+ hours!**

### Quality Improvement
- **Before:** Hit-or-miss results, often unrealistic
- **After:** Guaranteed professional quality
- **Learning Curve:** Users can study template parameters
- **Consistency:** Every project starts well

### Workflow Improvement
- **Before:** Generate → Tweak → Regenerate → Repeat
- **After:** Select → Create → Paint details → Export
- **Reduced Friction:** Fewer decisions = faster workflow

---

## 🚀 Implementation Phases

### Phase 1: Foundation (2-3 hours)
- [ ] Define `TerrainTemplate` structure
- [ ] Create `TemplateLibrary` class
- [ ] Create `TemplateGenerator` class
- [ ] Extend `StartingTemplate` enum

### Phase 2: Basic Templates (4-5 hours)
- [ ] Implement Unreal Hills template
- [ ] Implement Unreal Mountains template
- [ ] Implement Unreal Coastal template
- [ ] Test generation and parameter tuning

### Phase 3: UI Integration (3-4 hours)
- [ ] Update New Project dialog
- [ ] Add template category dropdown
- [ ] Add template preview panel
- [ ] Add "Use Recommended Settings" checkbox
- [ ] Wire up template selection to generator

### Phase 4: Complete Template Library (6-8 hours)
- [ ] All Unreal templates (Hills, Mountains, Coastal, Plains, Desert, Volcanic)
- [ ] All Unity templates (6 variants)
- [ ] All Godot templates (6 variants)
- [ ] Total: 18 templates

### Phase 5: Polish (2-3 hours)
- [ ] Template descriptions
- [ ] Tooltips
- [ ] Error handling
- [ ] Documentation
- [ ] User guide

**Total Estimated Time:** 17-23 hours

---

## 🎓 User Education

### Template Guide Document
Create `TEMPLATE_GUIDE.md` with:
- Overview of each template
- When to use each one
- Parameter explanations
- Customization tips
- Example screenshots (future)

### In-App Tooltips
- Template descriptions on hover
- Parameter explanations
- "Why this works" tips

---

## 🔮 Future Enhancements

### Phase 2: User Templates
- [ ] Save current terrain as custom template
- [ ] Share templates with community
- [ ] Template marketplace

### Phase 3: Template Variations
- [ ] "Snowy Mountains" variant
- [ ] "Tropical Coastal" variant
- [ ] "Rocky Plains" variant
- [ ] Seasonal variations

### Phase 4: Template Preview Thumbnails
- [ ] Generate preview images for each template
- [ ] Show in template selector
- [ ] 256x256 preview renders

### Phase 5: Template Randomization
- [ ] "Randomize Seed" button
- [ ] Variation slider (make it more/less extreme)
- [ ] Blend templates (70% Mountains + 30% Desert)

---

## ✅ Success Metrics

**How do we know this feature is successful?**
- ✅ 80%+ of new projects use templates (not Perlin Noise/Flat)
- ✅ User feedback: "Saved me hours!"
- ✅ Professional-looking terrain in exports
- ✅ Fewer "how do I make realistic terrain?" questions
- ✅ Users share template-based projects

---

## 📚 Related Documentation

- [TODO.md](TODO.md) - Phase 2.4 task breakdown
- [EngineScalingHelper.h](EngineScalingHelper.h) - Engine-specific height calculations
- [New Project Dialog](V076_MENU_SYSTEM_IMPLEMENTATION.md) - Current dialog implementation

---

**Status:** 📋 Design Complete, Ready for Implementation  
**Priority:** High - Significantly improves user experience  
**Estimated Effort:** 17-23 hours  
**Target Version:** V0.9.0 or V1.0
