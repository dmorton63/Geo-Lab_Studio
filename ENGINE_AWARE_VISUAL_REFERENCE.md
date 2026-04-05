# Engine-Aware Height System - Visual Reference

## System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                     USER INTERFACE (UIPanel)                     │
│                                                                   │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │ Paint Mode → Engine-Aware Presets                          │ │
│  │                                                             │ │
│  │  ☑ Use Engine Presets                                      │ │
│  │                                                             │ │
│  │  Target Engine:  [Unreal Engine ▼]                         │ │
│  │  Terrain Type:   [Mountains     ▼]                         │ │
│  │  Character:      [======180cm======] 50-300cm              │ │
│  │  Terrain Size:   [=====1000m=======] 100-5000m             │ │
│  │                                                             │ │
│  │  ┌──────────────────────────────────────────────────────┐ │ │
│  │  │ Unreal Engine 5                                       │ │ │
│  │  │ Character: 180cm (1.80m)                              │ │ │
│  │  │ Terrain Size: 1000m × 1000m                           │ │ │
│  │  │ Height Range: 0.0m to 99.0m                           │ │ │
│  │  │ Steep mountains with dramatic peaks                   │ │ │
│  │  │                                                        │ │ │
│  │  │ ✅ Active Range:                                      │ │ │
│  │  │    Normalized: 0.000 - 0.099                          │ │ │
│  │  │    Real-world: 0.0m - 99.0m                           │ │ │
│  │  └──────────────────────────────────────────────────────┘ │ │
│  │                                                             │ │
│  │  [Recalculate Range]                                        │ │
│  └────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
                                 ▼
┌─────────────────────────────────────────────────────────────────┐
│              EngineScalingHelper (Calculation Engine)            │
│                                                                   │
│  getEngineSpec(engine) → {height, capsule, angle}                │
│  getTerrainRange(type, charHeight) → {minM, maxM}                │
│  metersToNormalized(meters, size) → float (0.0-1.0)              │
│  calculateClampRange(params) → {minNorm, maxNorm}                │
└─────────────────────────────────────────────────────────────────┘
                                 ▼
┌─────────────────────────────────────────────────────────────────┐
│               LandscapeParameters (Data Storage)                 │
│                                                                   │
│  targetEngine: UnrealEngine / Unity / Godot / Custom             │
│  terrainType: Plains / Hills / Mountains / Coastal               │
│  characterHeight: 180.0f (cm)                                    │
│  terrainWorldSize: 1000.0f (m)                                   │
│  useEnginePresets: true                                          │
│  minHeight: 0.000 (calculated)                                   │
│  maxHeight: 0.099 (calculated)                                   │
└─────────────────────────────────────────────────────────────────┘
                                 ▼
┌─────────────────────────────────────────────────────────────────┐
│                  BrushTool (Clamping Logic)                      │
│                                                                   │
│  applyHeightClamping(newHeight, params)                          │
│    → Clamps painting to 0.000 - 0.099 range                      │
│    → Soft mode: eases near limits                                │
│    → Hard mode: immediate stop                                   │
└─────────────────────────────────────────────────────────────────┘
```

---

## Height Range Visualization

### Scale Comparison (180cm Character)

```
PLAINS (0-2m variation)
═══════════════════════════════════════════════════════════════
Character: 🧍 (1.8m)
Terrain:   ▁▂▁▁▂▁ (0-2m)
Feel:      Subtle, gentle rolling hills
Use:       Farmlands, grasslands, walkable everywhere

═══════════════════════════════════════════════════════════════
HILLS (0-14m variation)
═══════════════════════════════════════════════════════════════
Character: 🧍 (1.8m)
Terrain:   ▁▂▃▄▅▄▃▂▁ (0-14m)
Feel:      Noticeable elevation, climbable
Use:       Rolling landscapes, moderate challenge

═══════════════════════════════════════════════════════════════
MOUNTAINS (0-100m variation)
═══════════════════════════════════════════════════════════════
Character: 🧍 (1.8m)
Terrain:   ▁▂▃▄▅▆▇█▇▆▅▄▃▂▁ (0-100m)
Feel:      Epic peaks, dramatic vistas
Use:       Mountain ranges, requires paths/climbing

═══════════════════════════════════════════════════════════════
COASTAL (-4m to 6m variation)
═══════════════════════════════════════════════════════════════
Character: 🧍 (1.8m)
Water:     ▔▔▔▔▔▔▔▔▔▔ (0m = sea level)
Terrain:   ▁▂▃▄▅▆ (beach + cliffs)
Ocean:       ▂▁▁ (shallow floor)
Feel:      Beach with swimming areas
Use:       Coastal scenes, water gameplay

═══════════════════════════════════════════════════════════════
```

---

## Calculation Flow Diagram

```
USER INPUT
─────────────────────────────────────────────────────
┌─────────────┐  ┌─────────────┐  ┌─────────────┐
│   Engine    │  │   Terrain   │  │  Character  │
│   (UE5)     │  │   (Mountains)│  │   (180cm)   │
└──────┬──────┘  └──────┬──────┘  └──────┬──────┘
       │                │                │
       └────────────────┼────────────────┘
                        ▼
         ┌──────────────────────────────┐
         │  EngineScalingHelper         │
         │  Calculate Height Range      │
         └──────────────┬───────────────┘
                        ▼
STEP 1: Get Engine Spec
─────────────────────────────────────────────────────
       ┌────────────────────────────┐
       │ UE5 Default:                │
       │ - Character: 180cm (1.8m)   │
       │ - Capsule: 42cm             │
       │ - Walk Angle: 45°           │
       └────────────┬───────────────┘
                    ▼
STEP 2: Get Terrain Multiplier
─────────────────────────────────────────────────────
       ┌────────────────────────────┐
       │ Mountains:                  │
       │ - Multiplier: 55×           │
       │ - Description: Epic peaks   │
       └────────────┬───────────────┘
                    ▼
STEP 3: Calculate Real-World Range
─────────────────────────────────────────────────────
       ┌────────────────────────────┐
       │ maxHeight = 1.8m × 55       │
       │           = 99m             │
       │ minHeight = 0m              │
       └────────────┬───────────────┘
                    ▼
STEP 4: Normalize to 0.0-1.0
─────────────────────────────────────────────────────
       ┌────────────────────────────┐
       │ Terrain Size: 1000m         │
       │ maxNorm = 99 / 1000         │
       │         = 0.099 (9.9%)      │
       │ minNorm = 0 / 1000          │
       │         = 0.000 (0%)        │
       └────────────┬───────────────┘
                    ▼
OUTPUT (Applied to LandscapeParameters)
─────────────────────────────────────────────────────
       ┌────────────────────────────┐
       │ minHeight = 0.000           │
       │ maxHeight = 0.099           │
       │ enableHeightClamping = true │
       └────────────────────────────┘
```

---

## Terrain Type Multipliers (Reference)

```
CHARACTER HEIGHT: 180cm (1.8m)
═══════════════════════════════════════════════════════════════

┌────────────┬────────────┬──────────────┬────────────────────┐
│ Terrain    │ Multiplier │ Height Range │ Typical Use        │
├────────────┼────────────┼──────────────┼────────────────────┤
│ Plains     │    1.2×    │  0m -  2.1m  │ Farmlands, gentle  │
│ Hills      │    8.0×    │  0m - 14.4m  │ Rolling terrain    │
│ Mountains  │   55.0×    │  0m - 99.0m  │ Epic peaks         │
│ Coastal    │ -2× to 3×  │ -3.6m - 5.4m │ Beach + ocean      │
└────────────┴────────────┴──────────────┴────────────────────┘

NORMALIZATION EXAMPLES (1000m × 1000m terrain):
═══════════════════════════════════════════════════════════════

Plains:    2.1m / 1000m = 0.0021 (0.21% height variation)
Hills:    14.4m / 1000m = 0.0144 (1.44% height variation)
Mountains: 99m / 1000m = 0.0990 (9.90% height variation)
Coastal:   5.4m / 1000m = 0.0054 (0.54% above sea level)
          -3.6m / 1000m = -0.0036 (0.36% below sea level)
```

---

## Engine Comparison Chart

```
┌───────────────┬──────────┬─────────┬────────┬───────────────┐
│ Engine        │ Char. H. │ Capsule │ Angle  │ Notes         │
├───────────────┼──────────┼─────────┼────────┼───────────────┤
│ Unreal 5      │  180cm   │  42cm   │  45°   │ Third-Person  │
│ Unity         │  200cm   │  50cm   │  45°   │ CharController│
│ Godot 4       │  180cm   │  40cm   │  46°   │ CharacterBody │
│ Custom        │ Variable │ Variable│Variable│ User-defined  │
└───────────────┴──────────┴─────────┴────────┴───────────────┘
```

---

## Real-World Scale Reference

```
HUMAN HEIGHTS (for character reference)
═══════════════════════════════════════════════════════════════
  50cm  │ Toddler / Small creature
 100cm  │ Child (5-6 years old)
 120cm  │ Child (8-10 years old) ← Small protagonist
 140cm  │ Teen / Short character
 160cm  │ Average woman / Short man
 180cm  │ Average man ← Most game characters
 200cm  │ Tall character / Athlete
 220cm  │ Very tall / Basketball player
 250cm+ │ Giant / Monster / Mech

TERRAIN FEATURES (natural examples)
═══════════════════════════════════════════════════════════════
    2m  │ Single-story building, large boulder
    5m  │ Gentle hill, small cliff
   10m  │ Three-story building, noticeable hill
   20m  │ Six-story building, significant hill
   50m  │ 15-story building, small mountain
  100m  │ 30-story building, medium mountain
  500m  │ Small real-world mountain
 1000m+ │ Large real-world mountain (Everest: 8849m)
```

---

## Workflow Decision Tree

```
START: Creating Terrain for Game
    │
    ├─ Know your target engine?
    │   ├─ YES → Select from dropdown (UE/Unity/Godot)
    │   └─ NO  → Use Custom, estimate character size
    │
    ├─ What type of terrain?
    │   ├─ Mostly flat with gentle variation → Plains
    │   ├─ Rolling landscape, some challenge → Hills
    │   ├─ Dramatic peaks, epic vistas → Mountains
    │   ├─ Beach with water areas → Coastal
    │   └─ Something unique → Custom + adjust sliders
    │
    ├─ Know your terrain size in engine?
    │   ├─ YES → Input exact size (e.g., 2000m)
    │   └─ NO  → Use 1000m default, adjust later
    │
    ├─ Character different from engine default?
    │   ├─ YES → Adjust Character Height slider
    │   └─ NO  → Use engine default (auto-loaded)
    │
    └─ Check calculated range in UI
        ├─ Looks good? → START PAINTING! 🎨
        └─ Too extreme? → Adjust terrain type or size

RESULT: Natural-looking terrain perfectly scaled to your game! ✅
```

---

## Status Overlay in Action

```
┌─────────────────────────────────────┐
│ PAINT MODE                          │
├─────────────────────────────────────┤
│ Cursor: (128, 64)                   │
│ Height: 0.042                       │ ← Current height under cursor
├─────────────────────────────────────┤
│ Clamping: ON                        │
│ Range: 0.00 - 0.10                  │ ← Calculated from engine preset
├─────────────────────────────────────┤
│ Near Max                            │ ← ⚠️ Warning (yellow)
└─────────────────────────────────────┘

When painting mountains with UE5 preset:
- 0.042 = 42m elevation (42% of 99m max)
- "Near Max" means approaching 99m peak height
- Brush will ease-in (soft mode) as you get closer
```

---

🎉 **Visual Reference Complete!**

This diagram shows the complete data flow from user selection to brush clamping, making it easy to understand how character height translates to natural terrain ranges.
