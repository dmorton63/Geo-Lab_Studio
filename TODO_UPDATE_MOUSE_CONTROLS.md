# TODO.md Update - Advanced Mouse Controls Added

## What Was Added

### New Section: Phase 3.2.5 - Advanced Mouse Controls

A comprehensive paint mode workflow enhancement that allows real-time brush parameter adjustments using mouse buttons, reducing the need to constantly switch between viewport and parameter sliders.

---

## Feature Summary

### Middle Mouse Button (Wheel Button)
**Goal:** Adjust brush parameters without leaving the viewport

**Controls:**
- **Hold MMB + Drag Up/Down** → Adjust brush strength (1-100)
- **Hold MMB + Drag Left/Right** → Adjust brush radius (5-100)
- **Shift + MMB Drag** → Snap to preset values (10, 25, 50, 75, 100)

**Visual Feedback:**
- Real-time HUD overlay showing current values
- Semi-transparent yellow text near cursor
- Fades out 0.5s after release

---

### Extended Mouse Buttons (5+ Button Mice)

#### Button 4 (Back/Thumb Button)
**Primary:** Sample height under cursor
- Single click samples height at cursor position
- Sets flatten brush target height automatically
- Visual feedback: Brief flash showing value
- Perfect for matching adjacent terrain heights

**Alternative:** Hold + Drag for quick radius adjustment

#### Button 5 (Forward/Thumb Button)
**Primary:** Cycle through brush types
- Single click cycles: Raise → Lower → Smooth → Flatten → Raise
- Visual feedback: Brief overlay showing new brush type
- Faster than clicking dropdown menu

**Alternative:** Hold + Drag for quick strength adjustment

---

## Why This Matters

### Current Workflow Problem:
1. User paints in 2D viewport
2. Needs to adjust brush strength
3. Moves mouse to parameter panel
4. Adjusts slider
5. Moves mouse back to viewport
6. Loses artistic flow

### With Advanced Mouse Controls:
1. User paints in 2D viewport
2. Holds MMB + drags up to increase strength
3. Continues painting immediately
4. Artistic flow maintained

---

## Implementation Plan

### Phase 3.2.5 Tasks (TODO.md)

**Completed:**
- [x] Design specification
- [x] Add to TODO.md
- [x] Create implementation guide (ADVANCED_MOUSE_CONTROLS_SPEC.md)

**Next Steps:**
- [ ] Add settings to LandscapeParameters.h
- [ ] Implement MMB drag detection in main.cpp
- [ ] Create HUD overlay rendering system
- [ ] Implement Button 4/5 detection
- [ ] Add settings UI to UIPanel.cpp
- [ ] Test all interactions
- [ ] Document keyboard shortcuts

---

## Benefits

### For Digital Artists:
- ✅ Maintain artistic flow (no UI interruptions)
- ✅ Faster iteration (quick adjustments mid-stroke)
- ✅ Familiar workflow (similar to Photoshop/Blender)

### For Power Users:
- ✅ Utilize gaming mouse extra buttons
- ✅ Keyboard-free painting workflow
- ✅ Customizable button mappings (future)

### For Beginners:
- ✅ Optional feature (doesn't replace sliders)
- ✅ Visual HUD teaches control values
- ✅ Settings panel explains each button

---

## Technical Details

### Input Detection
- Uses GLFW mouse button constants
- Button detection: `glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_X)`
- Debouncing: 200ms to prevent multiple triggers
- Drag delta calculation for sensitivity

### Visual Feedback
- HUD: ImGui window with custom flags (no title, no resize, auto-size)
- Position: Offset from cursor to avoid blocking view
- Colors: Yellow text (#FFFF00) on semi-transparent background (alpha 0.7)
- Animation: Fade-out using alpha interpolation

### Settings Storage
New variables in `LandscapeParameters` struct:
```cpp
bool enableMMBDrag = true;
bool enableExtendedButtons = true;
bool showMouseControlHUD = true;
float mouseControlSensitivity = 1.0f;
```

---

## Related Documents

1. **TODO.md** (Updated)
   - Section 3.2.5: Advanced Mouse Controls (Paint Mode Focus)
   - Priority: High
   - Target: V0.8.0

2. **ADVANCED_MOUSE_CONTROLS_SPEC.md** (New)
   - Complete implementation guide
   - User stories
   - Technical specifications
   - Test cases

3. **COMPLETE_FEATURE_SUMMARY.md**
   - Will be updated when implemented
   - Add to "Paint/Sculpt System" section

---

## Timeline Estimate

### Phase 1: MMB Drag (2-3 hours)
- Input detection: 30 min
- Drag calculation: 30 min
- HUD overlay: 1 hour
- Testing: 1 hour

### Phase 2: Extended Buttons (1-2 hours)
- Button detection: 30 min
- Height sampling: 30 min
- Brush cycling: 30 min
- Testing: 30 min

### Phase 3: Settings UI (1 hour)
- UI layout: 30 min
- Integration: 20 min
- Testing: 10 min

**Total:** ~5-6 hours of focused development

---

## Success Criteria

### Functional
- ✅ MMB drag adjusts strength/radius smoothly
- ✅ HUD overlay appears and updates in real-time
- ✅ Button 4 samples height correctly
- ✅ Button 5 cycles brushes correctly
- ✅ Settings persist across sessions

### User Experience
- ✅ No input lag (< 16ms)
- ✅ HUD doesn't block view
- ✅ Controls feel natural and intuitive
- ✅ Doesn't interfere with existing controls

### Performance
- ✅ No FPS impact from HUD rendering
- ✅ No memory leaks from overlay system
- ✅ Clean input state management

---

## Future Extensions (Post-V0.8.0)

### Phase 3.2.6: Advanced Features
- Custom button remapping in settings
- Button mapping profiles (save/load)
- Gesture recognition (circles, lines for special actions)
- Tablet stylus button integration
- Brush falloff adjustment (Button 5 + scroll)

### Phase 3.2.7: Visual Enhancements
- Animated HUD transitions
- Brush preview ring showing current size
- Strength indicator (opacity ring)
- Height sampling visual (target line)

---

## References

**GLFW Mouse Buttons:**
- `GLFW_MOUSE_BUTTON_LEFT` (0)
- `GLFW_MOUSE_BUTTON_RIGHT` (1)
- `GLFW_MOUSE_BUTTON_MIDDLE` (2)
- `GLFW_MOUSE_BUTTON_4` (3) - Back/Thumb
- `GLFW_MOUSE_BUTTON_5` (4) - Forward/Thumb
- `GLFW_MOUSE_BUTTON_6` (5)
- `GLFW_MOUSE_BUTTON_7` (6)

**ImGui Mouse Buttons:**
- `ImGuiMouseButton_Left` (0)
- `ImGuiMouseButton_Right` (1)
- `ImGuiMouseButton_Middle` (2)
- ImGui only supports 3 buttons natively
- Extended buttons require GLFW direct access

---

**Document Created:** 2024  
**Feature Priority:** High  
**Target Version:** V0.8.0  
**Status:** Specification Complete, Ready for Implementation
