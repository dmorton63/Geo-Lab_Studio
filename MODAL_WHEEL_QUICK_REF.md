# Modal Mouse Wheel System - Quick Reference

## TL;DR
Press a key (R/S/F/L/H) to set what the mouse wheel controls. Scroll to adjust. No more menu diving while painting!

---

## The 5 Modes

| Key | Mode | Controls | Range | What It Does |
|-----|------|----------|-------|--------------|
| **R** | Radius | Brush Size | 5-100 | How big the brush circle is |
| **S** | Strength | Brush Intensity | 1-100 | How much to raise/lower |
| **F** | Feather | Edge Softness | 0.0-1.0 | 0=hard stamp, 1=soft blend |
| **L** | Start Level | Min Height | 0.0-1.0 | Only affect terrain below this |
| **H** | Max Height | Paint Ceiling | 0.0-1.0 | Brush can't exceed this height |

---

## How to Use

### Basic Workflow
```
1. Press R → Radius mode active
2. Scroll mouse wheel → Brush size changes
3. Press F → Feather mode active
4. Scroll mouse wheel → Brush softness changes
5. Keep painting without touching UI!
```

### Quick Tips
- **Shift + Scroll** = Fine adjustment (10% speed)
- **Ctrl + Scroll** = Coarse adjustment (2x speed)
- **Button 4** = Cycle modes (if you have programmable mouse)

---

## Visual Feedback

### Mode HUD (Always Visible)
```
┌────────────────────────────────┐
│ [R] Radius: 35.0      ← Yellow │
│ [S] Strength: 75.0             │
│ [F] Feather: 0.75              │
│ [L] Start Level: 0.2           │
│ [H] Max Height: 0.8            │
└────────────────────────────────┘
```
Active mode = Yellow  
Inactive modes = Gray

### Adjustment Overlay (Temporary)
Appears when you scroll, shows what's changing:
```
┌─────────────────────────┐
│ ▲ FEATHER MODE          │
│ ◄══════●═══════════►    │
│ 0.75 (75% soft edge)    │
└─────────────────────────┘
```

---

## New Features Explained

### Feather (F Mode)
**What:** Edge softness  
**When:** Want natural blending vs hard stamp  
**Example:**
- 0.0 = Hard circle (like cookie cutter)
- 0.5 = Medium soft (natural)
- 1.0 = Very soft (gradual fade)

### Start Level (L Mode)
**What:** Only affect terrain below this height  
**When:** Fill valleys without touching peaks  
**Example:**
- Terrain has valleys (0.2) and hills (0.7)
- Set Start Level to 0.5
- Use Raise brush → Only valleys fill up
- Hills remain untouched!

### Max Height (H Mode)
**What:** Ceiling/floor for current painting  
**When:** Want exact plateau heights  
**Example:**
- Set Max Height to 0.6
- Use Raise brush
- Paint area rises to 0.6 then stops
- Perfect flat plateau!

**Not the same as Height Clamping:**
- Height Clamping = Global safety (always on)
- Max Height = Artistic control (change per task)

---

## Common Workflows

### Creating Soft Hills
```
1. Press F → Feather mode
2. Scroll to 0.9 (very soft)
3. Press S → Strength mode
4. Scroll to 30 (gentle)
5. Paint with Raise brush
→ Smooth, natural hills
```

### Filling Valleys
```
1. Press L → Start Level mode
2. Scroll to 0.4 (above valley floor)
3. Select Raise brush
4. Paint over terrain
→ Valleys fill, peaks untouched
```

### Exact Plateaus
```
1. Press H → Max Height mode
2. Scroll to 0.55 (plateau height)
3. Select Raise brush
4. Paint area
→ Terrain rises to exactly 0.55 and stops
```

### Hard-Edge Roads
```
1. Press F → Feather mode
2. Scroll to 0.0 (hard edge)
3. Press H → Max Height mode
4. Scroll to 0.35 (road level)
5. Select Flatten brush
6. Paint road path
→ Perfectly flat road with crisp edges
```

---

## Why This Design?

### Compared to UI Sliders
**UI Sliders:**
- Move mouse to panel
- Find slider
- Adjust
- Move mouse back to viewport
- Breaks artistic flow

**Modal Wheel:**
- Press key
- Scroll wheel
- Keep painting
- Flow maintained ✅

### Compared to Hardcoded Controls
**Hardcoded (old design):**
- MMB + drag up = strength
- MMB + drag right = radius
- Can only control 2 things
- Can't add new parameters easily

**Modal (new design):**
- Any key can set any mode
- Scroll controls active mode
- Can control unlimited parameters
- Easy to add more modes ✅

---

## For Developers

### Adding a New Mode (Opacity Example)

**Step 1:** Add to enum
```cpp
enum class MouseWheelMode {
    Radius, Strength, Feather, StartLevel, MaxHeight,
    Opacity // ← NEW
};
```

**Step 2:** Add keyboard shortcut
```cpp
if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
    params.mouseWheelMode = MouseWheelMode::Opacity;
}
```

**Step 3:** Add scroll handler
```cpp
case MouseWheelMode::Opacity:
    params.brushOpacity += io.MouseWheel * 0.05f;
    params.brushOpacity = std::clamp(params.brushOpacity, 0.0f, 1.0f);
    break;
```

**Step 4:** Add to HUD
```cpp
ImGui::TextColored(isActive ? yellow : gray,
    "[O] Opacity: %.2f", params.brushOpacity);
```

**Done!** No new input methods, no UI redesign needed.

---

## Settings

### Where to Find (In Paint Mode)
```
Tools & Parameters Panel
  └─ Paint Tools
      └─ Mouse Wheel Modes
          ├─ Show mode HUD
          ├─ Show adjustment overlay
          └─ Current values (all modes)
```

### What You Can Configure
- Enable/disable mode HUD
- Enable/disable adjustment overlay
- View all current parameter values
- Reset to defaults

---

## Keyboard Shortcuts Cheat Sheet

**Mode Selection:**
- `R` - Radius mode
- `S` - Strength mode
- `F` - Feather mode
- `L` - Start Level mode
- `H` - Max Height mode

**Modifiers:**
- `Shift + Scroll` - Fine adjustment
- `Ctrl + Scroll` - Coarse adjustment

**Alternative:**
- `Button 4` - Cycle modes

**Other Paint Shortcuts:**
- `Ctrl+Z` - Undo
- `Ctrl+Y` - Redo
- `D` - Dump data

---

## FAQs

**Q: Can I use mouse buttons instead of keyboard?**  
A: Yes! Button 4 cycles through modes. But keyboard is faster once you learn R/S/F/L/H.

**Q: What if I forget which mode I'm in?**  
A: Look at the mode HUD (top-left corner). Active mode is yellow.

**Q: Can I disable this if I don't like it?**  
A: Yes! Just don't press mode keys. Regular scroll still works for zoom in 3D view. Or disable in settings.

**Q: Does this work outside paint mode?**  
A: No, only in paint mode. In Select mode, scroll wheel zooms the 3D camera as usual.

**Q: Can I remap the keys?**  
A: Not in V0.8.0, but we can add that in future if requested!

**Q: What about feather - is it the same as strength?**  
A: No! Strength = how much to raise/lower. Feather = how soft the brush edge is. Both important!

---

## Implementation Status

**Current Version:** V0.7.6  
**Feature Target:** V0.8.0  
**Status:** 📝 Design Complete

**Estimated Development Time:** 10-13 hours

**Phases:**
1. ✅ Design specification (DONE)
2. ✅ TODO.md updated (DONE)
3. ⏳ Implementation (Pending)
4. ⏳ Testing (Pending)
5. ⏳ Documentation update (Pending)

---

## Related Documents

- **MODAL_MOUSE_WHEEL_SYSTEM_SPEC.md** - Full technical specification
- **DESIGN_EVOLUTION_MOUSE_CONTROLS.md** - Why we chose this design
- **TODO.md** - Phase 3.2.5 implementation checklist
- **ADVANCED_MOUSE_CONTROLS_SPEC.md** - Original design (deprecated)

---

**Quick Reference Version:** 1.0  
**Last Updated:** 2024  
**For:** Landscape Designer V0.8.0
