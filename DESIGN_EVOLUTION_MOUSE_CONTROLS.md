# Design Evolution: Mouse Controls → Modal Mouse Wheel System

## Summary of Design Iteration

### Initial Design (v1.0 - Discarded)
**Hardcoded MMB Drag Controls**
- Middle mouse button + drag vertically = strength
- Middle mouse button + drag horizontally = radius
- Button 4 = sample height
- Button 5 = cycle brushes

**Problems:**
- ❌ Inflexible (only 2 parameters via MMB)
- ❌ Not scalable (what about feather? opacity? spacing?)
- ❌ Complex gestures (drag direction matters)
- ❌ Limited by input methods

---

### Revised Design (v2.0 - Current)
**Modal Mouse Wheel System**
- Keyboard shortcuts set mode: R/S/F/L/H
- Mouse wheel adjusts active mode parameter
- Button 4 cycles through modes (alternative)
- Visual HUD shows all modes + active one

**Advantages:**
- ✅ One input controls many parameters
- ✅ Infinitely scalable (add modes via enum)
- ✅ Simple gestures (just scroll wheel)
- ✅ Mnemonic shortcuts (easy to remember)
- ✅ Industry standard (Blender/ZBrush pattern)

---

## What Triggered the Revision?

### User Insight:
> "That mouse wheel can be used to adjust any number of items that the user is working on. So, maybe we need a little more tweaking on the concept before we attempt it."

> "Such as if I press F on the keyboard, the scroll wheel changes a feather value or like on my mouse I have programmable buttons on the side. Use one to cycle through mouse wheel modes!"

**Key Realization:** The mouse wheel is a single valuable input that should be modal/context-aware, not hardcoded.

---

## Design Comparison Table

| Aspect | v1.0 (Hardcoded) | v2.0 (Modal) |
|--------|------------------|--------------|
| **Input Method** | MMB drag + directions | Keyboard mode + wheel scroll |
| **Parameters Controlled** | 2 (radius, strength) | 5+ (radius, strength, feather, start level, max height) |
| **Scalability** | Low (limited by gestures) | High (add modes to enum) |
| **Discoverability** | Low (must learn gestures) | High (HUD shows all modes) |
| **Cognitive Load** | High (remember drag directions) | Low (mnemonic keys: F=Feather) |
| **Industry Precedent** | Photoshop-like | Blender/ZBrush-like |
| **Implementation Complexity** | Medium (drag detection) | Medium (mode switching) |
| **Future-Proofing** | Poor (what's next?) | Excellent (add modes easily) |

---

## New Features Introduced

### 1. Brush Feather/Falloff
**What it is:** Edge softness control (0.0 = hard stamp, 1.0 = gradual blend)

**Why it matters:**
- Creates natural-looking terrain transitions
- Essential for organic sculpting
- Standard feature in all professional tools

**Implementation:**
```cpp
// Smoothstep falloff based on feather value
float t = (distance - featherStart) / featherRegion;
float falloff = 1.0f - smoothstep(t);
```

---

### 2. Paint Start Level (Height Masking - Lower Bound)
**What it is:** Only affect terrain below a specific height

**Why it matters:**
- Fill valleys without touching peaks
- Level low areas while preserving hills
- Create "floor" for painting

**Use Case Example:**
```
Terrain: valleys at 0.2, hills at 0.7
Set Start Level: 0.5
Use Raise brush → Only valleys are affected
Result: Smooth valley floor, hills untouched
```

---

### 3. Max Paint Height (Height Ceiling/Floor)
**What it is:** Limit how high/low brush can paint (per-session)

**Why it matters:**
- Create exact plateau heights
- Paint to water level without going deeper
- Artistic control beyond global clamping

**Difference from Height Clamping:**
| Feature | Height Clamping | Max Paint Height |
|---------|----------------|------------------|
| **Scope** | Global (entire terrain) | Per-session (current painting) |
| **Purpose** | Safety (prevent catastrophes) | Artistry (precise control) |
| **Persistence** | Always active | Temporary (adjust per task) |
| **Example** | "Never go below 0.0 or above 1.0" | "Raise to 0.6 for this plateau, then change to 0.8 for next one" |

---

## Modal System Benefits

### Scalability Example
**Adding a new parameter (Opacity) in v1.0:**
- Need new gesture (Button 6 + drag?)
- Need new UI for new button
- Limited by available buttons

**Adding a new parameter (Opacity) in v2.0:**
```cpp
// 1. Add to enum
enum class MouseWheelMode {
    Radius, Strength, Feather, StartLevel, MaxHeight,
    Opacity // ← NEW
};

// 2. Add keyboard shortcut
if (key == GLFW_KEY_O) mode = MouseWheelMode::Opacity;

// 3. Add to HUD rendering
ImGui::Text("[O] Opacity: %.2f", params.opacity);

// Done! No new input methods needed.
```

---

## User Workflow Comparison

### v1.0 Workflow (Hardcoded)
```
1. User wants to adjust strength
2. Hold MMB + drag vertically
3. Watch parameter change
4. Release MMB
5. User wants to adjust radius
6. Hold MMB + drag horizontally
7. Watch parameter change
8. Release MMB
9. User wants to adjust feather → ❌ NOT POSSIBLE, must go to UI
```

### v2.0 Workflow (Modal)
```
1. User wants to adjust strength
2. Press S key (strength mode)
3. Scroll wheel up/down
4. Watch HUD update in real-time
5. User wants to adjust radius
6. Press R key (radius mode)
7. Scroll wheel up/down
8. Watch HUD update
9. User wants to adjust feather
10. Press F key (feather mode)
11. Scroll wheel up/down
12. Watch HUD update + brush cursor show gradient
✅ All accessible without leaving viewport!
```

---

## Implementation Estimates

### v1.0 (Hardcoded) - Estimated 5-6 hours
- MMB drag detection: 2 hours
- HUD overlay: 2 hours
- Button 4/5 detection: 1 hour
- Testing: 1 hour

### v2.0 (Modal) - Estimated 10-13 hours
- Mode system core: 3-4 hours
- Feather implementation: 2-3 hours
- Height masking (start level + max height): 3-4 hours
- UI & polish: 2 hours
- Testing: 1-2 hours

**Trade-off:** v2.0 takes 2x longer but provides 5x more functionality and infinite future scaling.

---

## Lessons Learned

### 1. Don't Hardcode Flexible Inputs
The mouse wheel is precious real estate. Making it modal/context-aware is worth the extra implementation complexity.

### 2. Look to Industry Standards
Blender's modal system (R for rotate, S for scale, G for grab) is well-proven. We adapted this pattern for brush parameters.

### 3. Plan for Scalability
Initial design only supported 2 parameters. User insight revealed need for many more (feather, opacity, spacing, etc.). Modal system handles this elegantly.

### 4. User Feedback is Gold
The suggestion to "press F and scroll wheel changes feather" completely transformed the design from limited to professional-grade.

---

## Next Steps

### Immediate (Design Phase)
- [x] Create MODAL_MOUSE_WHEEL_SYSTEM_SPEC.md (full technical spec)
- [x] Update TODO.md with revised Phase 3.2.5
- [x] Create DESIGN_EVOLUTION.md (this document)
- [ ] Get user approval on revised design

### Implementation (When Ready)
- [ ] Phase 1: Core modal system (mode enum, keyboard shortcuts, wheel handling)
- [ ] Phase 2: Brush feathering (falloff calculation, visual cursor)
- [ ] Phase 3: Height masking (start level + max height)
- [ ] Phase 4: UI & polish (HUD, overlays, settings panel)

### Future Enhancements
- Opacity mode (O key)
- Spacing mode (P key)
- Jitter mode (J key)
- Custom mode order/mapping
- Preset system (save/load mode configurations)

---

**Document Purpose:** Historical record of design evolution  
**Created:** 2024  
**Design Version:** v2.0 (Modal System)  
**Status:** Design complete, awaiting implementation approval
