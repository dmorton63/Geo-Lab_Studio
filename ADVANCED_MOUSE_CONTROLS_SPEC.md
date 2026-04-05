# Advanced Mouse Controls for Paint Mode - Implementation Guide

## Feature Overview
Enhance paint mode workflow by allowing real-time brush parameter adjustments using mouse buttons and dragging, reducing the need to constantly switch between viewport and parameter sliders.

---

## User Stories

### Story 1: Digital Artist Workflow
**As a** digital artist painting terrain details,  
**I want** to adjust brush strength without leaving the viewport,  
**So that** I can maintain artistic flow and make quick adjustments mid-stroke.

### Story 2: Height Sampling
**As a** terrain designer,  
**I want** to sample the height under my cursor with a button press,  
**So that** I can easily match heights when using the flatten brush.

### Story 3: Quick Brush Switching
**As a** power user with a gaming mouse,  
**I want** to use my extra mouse buttons for common operations,  
**So that** I can work faster without keyboard shortcuts.

---

## Feature Specifications

### 1. Middle Mouse Button (MMB) Drag Adjustments

#### Brush Strength Adjustment
- **Input:** Hold MMB + Drag Vertically
- **Behavior:**
  - Drag **up** = Increase strength
  - Drag **down** = Decrease strength
  - Range: 1.0 to 100.0 (matches current UI sliders)
  - Sensitivity: ~0.5 units per pixel dragged (configurable)
  
#### Brush Radius Adjustment
- **Input:** Hold MMB + Drag Horizontally  
- **Behavior:**
  - Drag **right** = Increase radius
  - Drag **left** = Decrease radius
  - Range: 5.0 to 100.0 (matches current UI sliders)
  - Sensitivity: ~0.3 units per pixel dragged (configurable)

#### Visual Feedback
- **HUD Overlay** (appears during MMB drag):
  ```
  ┌─────────────────┐
  │ Strength: 75.0  │  (Yellow text, semi-transparent background)
  │ Radius: 35.0    │
  └─────────────────┘
  ```
- Position: Near cursor (offset to avoid blocking view)
- Duration: Visible while MMB held, fade out 0.5s after release
- Font: Same as ImGui, but rendered directly to viewport

#### Modifier Keys
- **Shift + MMB Drag:** Snap to preset values
  - Strength presets: 10, 25, 50, 75, 100
  - Radius presets: 5, 10, 20, 35, 50, 75, 100
  - Snapping tolerance: ±5 pixels

---

### 2. Extended Mouse Buttons (Gaming Mice)

#### Button Mappings (Default)
| Button | GLFW Constant | Default Action | Alternative Mode |
|--------|---------------|----------------|------------------|
| MMB (Wheel) | `GLFW_MOUSE_BUTTON_MIDDLE` | Drag to adjust | - |
| Button 4 (Back) | `GLFW_MOUSE_BUTTON_4` | Sample height under cursor | Hold + Drag: Radius |
| Button 5 (Forward) | `GLFW_MOUSE_BUTTON_5` | Cycle brush type | Hold + Drag: Strength |
| Button 6 | `GLFW_MOUSE_BUTTON_6` | (Future: Undo) | - |
| Button 7 | `GLFW_MOUSE_BUTTON_7` | (Future: Redo) | - |

#### Button 4: Height Sampling
- **Single Click:**
  - Sample height value at cursor position
  - Set `params.flattenHeight` to sampled value
  - Visual feedback: Brief flash (0.3s) showing value
  - Console output: `"Height sampled: 0.473"`
  
- **Use Case:**
  - User wants to flatten an area to match existing terrain
  - Hovers cursor over target height
  - Clicks Button 4
  - Switches to Flatten brush (or already using it)
  - Paints with newly sampled target height

#### Button 5: Brush Type Cycling
- **Single Click:**
  - Cycle through: Raise → Lower → Smooth → Flatten → (back to Raise)
  - Visual feedback: Brief overlay showing new brush type
  - Console output: `"Brush changed to: Lower"`
  
- **Hold + Scroll Wheel (Future):**
  - Adjust brush falloff/softness
  - Not implemented in Phase 3.2.5, saved for Phase 3.2.6

---

### 3. Settings/Configuration UI

Add new section to Paint Mode parameters panel:

```
┌─────────────────────────────────────────┐
│ ▼ Advanced Mouse Controls               │
├─────────────────────────────────────────┤
│ [x] Enable MMB drag adjustments         │
│                                          │
│ MMB Vertical:   [Brush Strength ▼]      │
│ MMB Horizontal: [Brush Radius   ▼]      │
│                                          │
│ Sensitivity: [========|====] 1.0        │
│                                          │
│ [x] Show HUD overlay during adjustment  │
│                                          │
│ ─────────────────────────────────────── │
│                                          │
│ [x] Enable extended mouse buttons       │
│                                          │
│ Button 4: [Sample Height        ▼]      │
│ Button 5: [Cycle Brush Type     ▼]      │
│                                          │
│ [ Reset to Defaults ]                   │
└─────────────────────────────────────────┘
```

**Settings Storage:**
- Add to `LandscapeParameters` struct:
  ```cpp
  bool enableMMBDrag = true;
  bool enableExtendedButtons = true;
  bool showMouseControlHUD = true;
  float mouseControlSensitivity = 1.0f;
  
  // Button mappings (future: allow remapping)
  enum class MouseButtonAction {
      None, SampleHeight, CycleBrush, Undo, Redo
  };
  MouseButtonAction button4Action = MouseButtonAction::SampleHeight;
  MouseButtonAction button5Action = MouseButtonAction::CycleBrush;
  ```

---

## Technical Implementation

### Phase 1: MMB Drag System

#### Input Detection (main.cpp)
```cpp
// In paint mode interaction section
static bool mmb_dragging = false;
static ImVec2 mmb_drag_start;
static float original_strength = 0.0f;
static float original_radius = 0.0f;

if (params.paintMode && is2DViewHovered) {
    // Detect MMB press
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) {
        mmb_dragging = true;
        mmb_drag_start = ImGui::GetMousePos();
        original_strength = params.brushStrength;
        original_radius = params.brushRadius;
    }
    
    // Handle MMB drag
    if (mmb_dragging && ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
        ImVec2 current_pos = ImGui::GetMousePos();
        float deltaX = current_pos.x - mmb_drag_start.x;
        float deltaY = current_pos.y - mmb_drag_start.y;
        
        // Vertical drag = strength
        params.brushStrength = original_strength - (deltaY * 0.5f * params.mouseControlSensitivity);
        params.brushStrength = std::clamp(params.brushStrength, 1.0f, 100.0f);
        
        // Horizontal drag = radius
        params.brushRadius = original_radius + (deltaX * 0.3f * params.mouseControlSensitivity);
        params.brushRadius = std::clamp(params.brushRadius, 5.0f, 100.0f);
        
        // Snap to presets if Shift held
        if (ImGui::GetIO().KeyShift) {
            // Snap logic here
        }
    }
    
    // Release MMB
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Middle)) {
        mmb_dragging = false;
    }
}
```

#### HUD Overlay Rendering
```cpp
// After paint mode interaction, before ImGui::End()
if (mmb_dragging) {
    ImVec2 cursor_pos = ImGui::GetMousePos();
    ImVec2 hud_pos = ImVec2(cursor_pos.x + 20, cursor_pos.y - 40);
    
    ImGui::SetNextWindowPos(hud_pos);
    ImGui::SetNextWindowBgAlpha(0.7f);
    ImGui::Begin("##MouseControlHUD", nullptr, 
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize |
                 ImGuiWindowFlags_NoSavedSettings);
    
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Strength: %.1f", params.brushStrength);
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Radius:   %.1f", params.brushRadius);
    
    ImGui::End();
}
```

### Phase 2: Extended Mouse Buttons

#### Button Detection (main.cpp)
```cpp
// Add to input handling section
if (params.enableExtendedButtons && params.paintMode && is2DViewHovered) {
    // Button 4: Sample height
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_4) == GLFW_PRESS) {
        static double lastSampleTime = 0.0;
        double currentTime = glfwGetTime();
        
        if (currentTime - lastSampleTime > 0.2) { // Debounce
            // Sample height at cursor
            if (heightmapX >= 0 && heightmapX < rawHeight.width() &&
                heightmapY >= 0 && heightmapY < rawHeight.height()) {
                
                params.flattenHeight = rawHeight(heightmapX, heightmapY);
                std::cout << "Height sampled: " << params.flattenHeight << std::endl;
                
                // TODO: Add visual flash effect
                lastSampleTime = currentTime;
            }
        }
    }
    
    // Button 5: Cycle brush type
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_5) == GLFW_PRESS) {
        static double lastCycleTime = 0.0;
        double currentTime = glfwGetTime();
        
        if (currentTime - lastCycleTime > 0.2) { // Debounce
            int currentBrush = static_cast<int>(params.brushType);
            currentBrush = (currentBrush + 1) % 4; // Cycle 0-3
            params.brushType = static_cast<BrushType>(currentBrush);
            
            const char* brushNames[] = {"Raise", "Lower", "Smooth", "Flatten"};
            std::cout << "Brush changed to: " << brushNames[currentBrush] << std::endl;
            
            lastCycleTime = currentTime;
        }
    }
}
```

### Phase 3: Settings UI (UIPanel.cpp)

```cpp
void UIPanel::renderMouseControlSettings(LandscapeParameters& params) {
    if (ImGui::TreeNode("Advanced Mouse Controls")) {
        ImGui::Checkbox("Enable MMB drag adjustments", &params.enableMMBDrag);
        
        if (params.enableMMBDrag) {
            ImGui::Spacing();
            ImGui::SliderFloat("Sensitivity", &params.mouseControlSensitivity, 0.1f, 3.0f);
            ImGui::Checkbox("Show HUD overlay", &params.showMouseControlHUD);
        }
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Checkbox("Enable extended mouse buttons", &params.enableExtendedButtons);
        
        if (params.enableExtendedButtons) {
            ImGui::Spacing();
            ImGui::TextWrapped("Button 4 (Back): Sample height under cursor");
            ImGui::TextWrapped("Button 5 (Forward): Cycle brush type");
        }
        
        ImGui::Spacing();
        
        if (ImGui::Button("Reset to Defaults", ImVec2(-1, 0))) {
            params.enableMMBDrag = true;
            params.enableExtendedButtons = true;
            params.showMouseControlHUD = true;
            params.mouseControlSensitivity = 1.0f;
        }
        
        ImGui::TreePop();
    }
}
```

---

## Testing Plan

### Test Cases

#### TC1: MMB Vertical Drag (Strength)
1. Enter paint mode
2. Hover over 2D viewport
3. Hold MMB + drag up 100 pixels
4. **Expected:** Strength increases by ~50 units
5. **Verify:** HUD shows updated value, parameter slider syncs

#### TC2: MMB Horizontal Drag (Radius)
1. Enter paint mode
2. Hover over 2D viewport
3. Hold MMB + drag right 100 pixels
4. **Expected:** Radius increases by ~30 units
5. **Verify:** HUD shows updated value, brush cursor updates size

#### TC3: Shift+MMB Snap
1. Enter paint mode
2. Hold Shift + MMB + drag vertically
3. **Expected:** Strength snaps to nearest preset (10, 25, 50, 75, 100)
4. **Verify:** Value jumps to preset when within tolerance

#### TC4: Button 4 Height Sampling
1. Enter paint mode
2. Generate terrain with varied heights
3. Hover cursor over high point (e.g., 0.8)
4. Click Button 4
5. **Expected:** Console shows "Height sampled: 0.8", flattenHeight set to 0.8
6. Switch to Flatten brush, verify it paints at 0.8

#### TC5: Button 5 Brush Cycling
1. Enter paint mode with Raise brush active
2. Click Button 5
3. **Expected:** Brush changes to Lower
4. Click Button 5 again
5. **Expected:** Brush changes to Smooth
6. Continue cycling through Flatten → Raise

#### TC6: Settings Persistence
1. Disable MMB drag in settings
2. Try MMB drag
3. **Expected:** No adjustment occurs
4. Re-enable MMB drag
5. **Expected:** Adjustment works again

---

## Future Enhancements (Phase 3.2.6+)

### Brush Falloff/Softness
- **Input:** Hold Button 5 + Scroll Wheel
- **Behavior:** Adjust brush edge softness (hard edge vs smooth falloff)
- **UI:** Add falloff slider to brush settings

### Custom Button Remapping
- **UI:** Dropdown for each mouse button allowing action selection
- **Actions:** None, Sample Height, Cycle Brush, Undo, Redo, Toggle 3D View, etc.
- **Profiles:** Save/load button mapping profiles

### Gesture System
- **Complex Gestures:**
  - MMB + Circle motion = Smooth entire area under circle
  - MMB + Line motion = Create ramp between endpoints
  - MMB + Double-tap = Reset brush to defaults

---

## Dependencies

### GLFW Mouse Button Constants
```cpp
GLFW_MOUSE_BUTTON_MIDDLE  // 2
GLFW_MOUSE_BUTTON_4       // 3 (Back/Thumb)
GLFW_MOUSE_BUTTON_5       // 4 (Forward/Thumb)
GLFW_MOUSE_BUTTON_6       // 5
GLFW_MOUSE_BUTTON_7       // 6
```

### Files to Modify
1. `LandscapeParameters.h` - Add settings variables
2. `main.cpp` - Add input detection and HUD rendering
3. `UIPanel.cpp` - Add settings UI section
4. `TODO.md` - Track implementation progress ✅ (already updated)

---

## Success Metrics

### User Experience
- ✅ 50% reduction in parameter panel interactions during painting
- ✅ Faster workflow for digital artists (subjective feedback)
- ✅ Positive user reviews on ergonomics

### Technical
- ✅ Input lag < 16ms (1 frame at 60 FPS)
- ✅ HUD rendering doesn't impact FPS
- ✅ No conflicts with existing controls
- ✅ 0 bugs related to button detection

---

**Document Version:** 1.0  
**Feature Target:** V0.8.0  
**Priority:** High (Phase 3.2.5)  
**Status:** Design Complete, Implementation Pending
