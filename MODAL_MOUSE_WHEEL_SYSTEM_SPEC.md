# Modal Mouse Wheel System - Revised Specification

## Overview
A flexible, mode-based system where the mouse wheel's function changes based on the active mode, selected via keyboard shortcuts or mouse button cycling. This provides quick access to multiple brush parameters without cluttering the UI.

---

## Design Philosophy

### Problem with Original Approach
- **Hardcoded Controls:** MMB drag for strength/radius is inflexible
- **Limited Scalability:** Can't easily add new parameters
- **Cognitive Load:** Users must remember complex gestures

### New Approach: Modal System
- **One Input, Many Functions:** Mouse wheel controls different parameters based on mode
- **Discoverable:** Keyboard shortcuts are mnemonic (F = Feather, S = Strength)
- **Scalable:** Easy to add new modes without new input methods
- **Familiar:** Similar to Blender, ZBrush, Substance Painter

---

## Mode System Design

### Mode List

| Mode | Key | Mouse Button Cycle Order | Controls | Range | Description |
|------|-----|--------------------------|----------|-------|-------------|
| **Radius** | R | 1st | Brush Size | 5-100 | How large the brush circle is |
| **Strength** | S | 2nd | Brush Intensity | 1-100 | How much to raise/lower per stroke |
| **Feather** | F | 3rd | Brush Falloff | 0.0-1.0 | Edge softness (0=hard, 1=soft) |
| **Start Level** | L | 4th | Min Affected Height | 0.0-1.0 | Only affect terrain below this height |
| **Max Height** | H | 5th | Max Paint Ceiling | 0.0-1.0 | Brush can't exceed this height |

### Mode Activation

#### Keyboard Shortcuts (Primary)
```cpp
if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
    params.mouseWheelMode = MouseWheelMode::Radius;
}
if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    params.mouseWheelMode = MouseWheelMode::Strength;
}
if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
    params.mouseWheelMode = MouseWheelMode::Feather;
}
if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
    params.mouseWheelMode = MouseWheelMode::StartLevel;
}
if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
    params.mouseWheelMode = MouseWheelMode::MaxHeight;
}
```

#### Mouse Button Cycling (Secondary)
```cpp
if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_4) == GLFW_PRESS) {
    // Cycle through modes
    int currentMode = static_cast<int>(params.mouseWheelMode);
    currentMode = (currentMode + 1) % 5; // 5 modes total
    params.mouseWheelMode = static_cast<MouseWheelMode>(currentMode);
}
```

### Mouse Wheel Behavior

#### Scroll Direction
- **Scroll Up** = Increase value
- **Scroll Down** = Decrease value
- **Shift + Scroll** = Fine adjustment (0.1x speed)
- **Ctrl + Scroll** = Coarse adjustment (2x speed)

#### Sensitivity per Mode
```cpp
// In mouse wheel handler
float sensitivity = 1.0f;
if (ImGui::GetIO().KeyShift) sensitivity = 0.1f;
if (ImGui::GetIO().KeyCtrl) sensitivity = 2.0f;

switch (params.mouseWheelMode) {
    case MouseWheelMode::Radius:
        params.brushRadius += io.MouseWheel * 5.0f * sensitivity;
        params.brushRadius = std::clamp(params.brushRadius, 5.0f, 100.0f);
        break;
    
    case MouseWheelMode::Strength:
        params.brushStrength += io.MouseWheel * 5.0f * sensitivity;
        params.brushStrength = std::clamp(params.brushStrength, 1.0f, 100.0f);
        break;
    
    case MouseWheelMode::Feather:
        params.brushFeather += io.MouseWheel * 0.05f * sensitivity;
        params.brushFeather = std::clamp(params.brushFeather, 0.0f, 1.0f);
        break;
    
    case MouseWheelMode::StartLevel:
        params.paintStartLevel += io.MouseWheel * 0.05f * sensitivity;
        params.paintStartLevel = std::clamp(params.paintStartLevel, 0.0f, 1.0f);
        break;
    
    case MouseWheelMode::MaxHeight:
        params.maxPaintHeight += io.MouseWheel * 0.05f * sensitivity;
        params.maxPaintHeight = std::clamp(params.maxPaintHeight, 0.0f, 1.0f);
        break;
}
```

---

## Visual Feedback System

### Mode HUD (Always Visible in Paint Mode)

**Position:** Top-left corner of 2D viewport  
**Design:** Compact, non-intrusive

```
┌────────────────────────────────┐
│ [R] Radius: 35.0               │  ← Current mode highlighted
│ [S] Strength: 75.0             │
│ [F] Feather: 0.75              │
│ [L] Start Level: 0.2           │
│ [H] Max Height: 0.8            │
└────────────────────────────────┘
```

**Rendering:**
```cpp
ImGui::SetNextWindowPos(ImVec2(contentMin2D.x + 10, contentMin2D.y + 10));
ImGui::SetNextWindowBgAlpha(0.8f);
ImGui::Begin("##ModeHUD", nullptr, ImGuiWindowFlags_NoTitleBar | ...);

// Highlight active mode
ImVec4 normalColor(0.7f, 0.7f, 0.7f, 1.0f);
ImVec4 activeColor(1.0f, 1.0f, 0.0f, 1.0f); // Yellow

ImGui::TextColored(params.mouseWheelMode == MouseWheelMode::Radius ? activeColor : normalColor, 
                   "[R] Radius: %.1f", params.brushRadius);
ImGui::TextColored(params.mouseWheelMode == MouseWheelMode::Strength ? activeColor : normalColor, 
                   "[S] Strength: %.1f", params.brushStrength);
// ... etc

ImGui::End();
```

### Adjustment Feedback (Temporary Overlay)

**Appears When:** Mouse wheel scrolled  
**Duration:** 1 second after last scroll  
**Position:** Near cursor

```
┌─────────────────────────┐
│ ▲ FEATHER MODE          │
│ ◄══════●═══════════►    │
│ 0.75 (75% soft edge)    │
└─────────────────────────┘
```

**Animation:**
- Fade in: 0.1s
- Hold: While scrolling + 1s
- Fade out: 0.3s

---

## New Parameters Implementation

### 1. Brush Feather/Falloff

**Purpose:** Control edge softness for natural blending

**Implementation in BrushTool:**
```cpp
float BrushTool::calculateFalloff(float distance, float radius, float feather) {
    // distance: pixels from brush center
    // radius: brush radius
    // feather: 0.0 (hard) to 1.0 (soft)
    
    if (distance >= radius) return 0.0f;
    
    if (feather < 0.01f) {
        // Hard edge (step function)
        return 1.0f;
    } else {
        // Soft edge (smoothstep with feather control)
        float normalizedDist = distance / radius;
        float featherStart = 1.0f - feather;
        
        if (normalizedDist < featherStart) {
            return 1.0f; // Full strength in center
        } else {
            // Smoothstep falloff in outer ring
            float t = (normalizedDist - featherStart) / feather;
            return 1.0f - (t * t * (3.0f - 2.0f * t)); // Smoothstep
        }
    }
}
```

**Visual Representation:**
- Brush cursor shows gradient ring
- Inner solid circle = full strength region
- Outer gradient ring = falloff region

### 2. Paint Start Level (Height Masking - Lower Bound)

**Purpose:** Only affect terrain below a certain height  
**Use Cases:**
- Fill valleys without touching peaks
- Level low areas while preserving hills
- Create "floors" in terrain

**Implementation:**
```cpp
void BrushTool::applyBrush(...) {
    for (each pixel in brush radius) {
        float currentHeight = heightmap(x, y);
        
        // Height masking check
        if (currentHeight > params.paintStartLevel) {
            continue; // Skip pixels above start level
        }
        
        // Apply brush effect only to pixels below start level
        float newHeight = calculateBrushEffect(...);
        heightmap(x, y) = newHeight;
    }
}
```

**Visual Feedback:**
- Overlay line showing start level on heightmap
- Brush cursor changes color when over masked area (red tint = "won't affect")

### 3. Max Paint Height (Height Ceiling/Floor)

**Purpose:** Limit how high/low the brush can paint  
**Use Cases:**
- Raise terrain to a specific plateau height, then stop
- Lower terrain to create water level without going deeper
- Create terraced effects

**Difference from Height Clamping:**
- **Height Clamping:** Global terrain limits (prevents catastrophes)
- **Max Paint Height:** Per-painting session limit (artistic control)

**Implementation:**
```cpp
void BrushTool::applyBrush(...) {
    for (each pixel in brush radius) {
        float currentHeight = heightmap(x, y);
        float newHeight = calculateBrushEffect(...);
        
        // Apply max paint height limit
        switch (params.brushType) {
            case BrushType::Raise:
                newHeight = std::min(newHeight, params.maxPaintHeight);
                break;
            
            case BrushType::Lower:
                newHeight = std::max(newHeight, params.maxPaintHeight);
                break;
            
            // Smooth and Flatten respect the limit as well
        }
        
        heightmap(x, y) = newHeight;
    }
}
```

**Visual Feedback:**
- Overlay line showing max height on heightmap (different color from start level)
- Brush cursor shows "ceiling" indicator when at limit

---

## UI Integration

### Parameters Panel (Paint Mode)

Add new section:

```
┌─────────────────────────────────────────┐
│ ▼ Mouse Wheel Modes                     │
├─────────────────────────────────────────┤
│ Active Mode: [Radius ▼]                 │
│   (Use R/S/F/L/H keys or Button 4)      │
│                                          │
│ [x] Show mode HUD in viewport           │
│ [x] Show adjustment overlay              │
│                                          │
│ ─────────────────────────────────────── │
│                                          │
│ Current Values:                          │
│   Radius:      [====●=====] 35.0        │
│   Strength:    [========●=] 75.0        │
│   Feather:     [======●===] 0.75        │
│   Start Level: [==●=======] 0.2         │
│   Max Height:  [========●=] 0.8         │
│                                          │
│ [ Reset All to Defaults ]               │
└─────────────────────────────────────────┘

┌─────────────────────────────────────────┐
│ ▼ Height Masking (Advanced)             │
├─────────────────────────────────────────┤
│ [x] Enable Start Level Masking          │
│     Only affect terrain below:          │
│     [==●=======] 0.2                    │
│                                          │
│ [x] Enable Max Paint Height             │
│     Limit painting to:                   │
│     [========●=] 0.8                    │
│                                          │
│ [ ] Show masking overlay on viewport    │
│                                          │
│ Info: Start Level = floor, Max Height = │
│       ceiling for your current paint    │
│       session. Different from global    │
│       height clamping!                   │
└─────────────────────────────────────────┘
```

### Keyboard Shortcuts Panel Update

Add to shortcuts reference:

```
Paint Mode Shortcuts:
  R - Set mouse wheel to Radius mode
  S - Set mouse wheel to Strength mode
  F - Set mouse wheel to Feather mode
  L - Set mouse wheel to Start Level mode
  H - Set mouse wheel to Max Height mode
  
  Mouse Wheel - Adjust active mode parameter
  Shift + Wheel - Fine adjustment (10% speed)
  Ctrl + Wheel - Coarse adjustment (2x speed)
  
  Button 4 - Cycle through modes
```

---

## Implementation Plan (Revised)

### Phase 1: Core Modal System (3-4 hours)

**Step 1.1: Data Structures**
```cpp
// In LandscapeParameters.h
enum class MouseWheelMode {
    Radius = 0,
    Strength = 1,
    Feather = 2,
    StartLevel = 3,
    MaxHeight = 4
};

// In LandscapeParameters struct:
MouseWheelMode mouseWheelMode = MouseWheelMode::Radius;
float brushFeather = 0.5f;           // NEW
float paintStartLevel = 0.0f;        // NEW
float maxPaintHeight = 1.0f;         // NEW
bool enableStartLevelMasking = false; // NEW
bool enableMaxPaintHeight = false;    // NEW
bool showModeHUD = true;
bool showAdjustmentOverlay = true;
```

**Step 1.2: Input Detection (main.cpp)**
- Keyboard mode selection (R/S/F/L/H keys)
- Mouse button mode cycling (Button 4)
- Mouse wheel parameter adjustment
- Modifier key handling (Shift/Ctrl)

**Step 1.3: Visual HUD Rendering**
- Mode HUD (top-left, always visible)
- Adjustment overlay (near cursor, temporary)
- Fade animations

### Phase 2: Brush Feathering (2-3 hours)

**Step 2.1: Falloff Calculation**
- Implement smoothstep falloff function
- Integrate with existing brush code

**Step 2.2: Visual Brush Cursor Update**
- Show gradient ring for feather visualization
- Inner circle = full strength
- Outer ring = falloff region

### Phase 3: Height Masking (3-4 hours)

**Step 3.1: Start Level Masking**
- Implement height check before brush application
- Visual overlay showing masked regions

**Step 3.2: Max Paint Height**
- Implement ceiling/floor limits per brush type
- Visual overlay showing height limit

**Step 3.3: Viewport Overlays**
- Horizontal line showing start level (blue)
- Horizontal line showing max height (red)
- Gradient/hatch pattern showing masked areas

### Phase 4: UI & Polish (2 hours)

**Step 4.1: Parameters Panel**
- Mode selection dropdown
- All parameter sliders
- Height masking section

**Step 4.2: Keyboard Shortcuts**
- Update shortcuts panel
- Add tooltips

**Step 4.3: Testing**
- Test all modes
- Test modifier keys
- Test visual feedback

**Total Estimated Time:** 10-13 hours

---

## Advanced Use Cases

### Use Case 1: Plateau Creation
```
1. Set Max Paint Height to 0.6 (plateau level)
2. Press H key (max height mode active)
3. Scroll wheel to fine-tune plateau height
4. Select Raise brush
5. Paint terrain - automatically stops at 0.6
6. Result: Perfect flat plateau at exact height
```

### Use Case 2: Valley Filling
```
1. Set Start Level to 0.4 (only affect below this)
2. Press L key (start level mode active)
3. Scroll wheel to set threshold
4. Select Raise brush
5. Paint valleys - hills remain untouched
6. Result: Valleys filled, peaks preserved
```

### Use Case 3: Soft Sculpting
```
1. Set Feather to 0.9 (very soft)
2. Press F key (feather mode active)
3. Scroll wheel to adjust softness while painting
4. Result: Natural, organic terrain with smooth transitions
```

### Use Case 4: Precise Road Carving
```
1. Set Feather to 0.0 (hard edge)
2. Set Max Paint Height to 0.35 (road level)
3. Select Flatten brush
4. Paint road path
5. Result: Perfectly flat road with hard edges
```

---

## Benefits of Modal System

### Scalability
- ✅ Easy to add new modes (just extend enum)
- ✅ No new input methods needed
- ✅ UI grows naturally

### Discoverability
- ✅ Mnemonic keyboard shortcuts (F = Feather)
- ✅ Visual HUD shows all modes at once
- ✅ Clear feedback for active mode

### Flexibility
- ✅ Users choose preferred input (keyboard or mouse)
- ✅ Modifier keys provide fine control
- ✅ Can be disabled entirely if not wanted

### Professional Workflow
- ✅ Matches industry standards (Blender, ZBrush)
- ✅ Keeps hands on painting, not UI
- ✅ Fast iteration without menu diving

---

## Future Enhancements (Post-V0.8.0)

### Additional Modes
- **Opacity Mode** (O key): Adjust brush transparency (blend with existing terrain)
- **Spacing Mode** (P key): Adjust stroke spacing (for textured effects)
- **Jitter Mode** (J key): Add randomness to brush

### Visual Enhancements
- Animated mode transitions
- Color-coded mode indicators
- Brush preview showing parameter effects
- Real-time terrain preview of height limits

### Power User Features
- Custom mode order (rearrange cycle sequence)
- Save mode presets
- Quick preset buttons (1-9 keys)
- Macro recording (record adjustment sequences)

---

**Document Version:** 2.0 (Revised from 1.0)  
**Feature Target:** V0.8.0  
**Priority:** High (Phase 3.2.5)  
**Status:** Design Complete (Revised), Ready for Implementation
