# 🎮 Quick Reference Card - Engine-Aware Terrain Creation

## 30-Second Workflow

```
1. Enable Paint Mode ✓
2. Open "Engine-Aware Presets" ✓
3. Select your engine & terrain type ✓
4. Paint! Export! Import! ✓
5. Perfect scale on first try! 🎉
```

---

## Engine Presets at a Glance

| Engine | Character | Best For |
|--------|-----------|----------|
| **Unreal 5** | 180cm | Third-person action, AAA games |
| **Unity** | 200cm | General purpose, indie games |
| **Godot 4** | 180cm | Open-source projects |
| **Custom** | You choose | Unique games, prototypes |

---

## Terrain Types Quick Pick

| Type | Height Range | When to Use |
|------|--------------|-------------|
| 🏞️ **Plains** | 0-2m | Farmlands, race tracks, gentle areas |
| ⛰️ **Hills** | 0-14m | Rolling landscapes, hiking gameplay |
| 🏔️ **Mountains** | 0-100m | Epic vistas, climbing challenges |
| 🏖️ **Coastal** | -4m to 6m | Beaches, swimming, ocean scenes |

---

## What You Get Automatically

✅ Natural height limits (no more Mt. Everest accidents!)  
✅ Character-proportional scaling (feels right in-game)  
✅ Real-time clamping (can't paint beyond safe range)  
✅ Visual feedback (see height as you paint)  
✅ Warning system (alerts when near limits)  

---

## Common Questions

**Q: Will this work with my 150cm character?**  
A: Yes! Adjust "Character Height" slider to 150cm. Done!

**Q: My terrain is 4000m, not 1000m?**  
A: Adjust "Terrain Size" slider to 4000m. Recalculates instantly!

**Q: Can I still use manual min/max?**  
A: Yes! Turn off "Use Engine Presets" for manual mode.

**Q: What if I don't know my engine yet?**  
A: Pick any preset now, change later. Or use Custom!

**Q: Does this slow down painting?**  
A: Nope! Zero performance impact. Math is instant.

---

## Pro Tips

💡 **Set presets BEFORE painting** - Consistent scale from start  
💡 **Match your engine's terrain size** - Check project settings  
💡 **Test early** - Export → Import → Verify after 5 minutes of work  
💡 **Save character height** - Write it down for future maps  
💡 **Use soft mode** - Natural feel for organic terrain  

---

## Keyboard Shortcuts

| Key | Action |
|-----|--------|
| **Left Click + Drag** | Paint with current brush |
| **Ctrl + Z** | Undo |
| **Ctrl + Y** | Redo |
| **D** (future) | Debug data dump |

---

## Status Overlay Legend

```
┌────────────────────┐
│ PAINT MODE         │  ← Paint mode active
│ Cursor: (128, 64)  │  ← Heightmap coordinates
│ Height: 0.042      │  ← Current normalized height
│ Clamping: ON       │  ← Safety enabled
│ Range: 0.00-0.10   │  ← Active limits
│ Near Max           │  ← ⚠️ Warning
└────────────────────┘
```

**Colors:**
- 🟢 **Normal** - Safe to paint
- 🟡 **Yellow** - Approaching limit (within 5%)
- 🔴 **Red** - AT limit (can't go further)

---

## Export & Import Guide

### **Unreal Engine 5**
1. Export as **RAW** (16-bit)
2. In UE: Landscape → Import
3. Set scale to match "Terrain Size"
4. Verify character can traverse terrain

### **Unity**
1. Export as **RAW** (16-bit)
2. In Unity: Assets → Import → Raw
3. Terrain Settings → Match size
4. Test with Character Controller

### **Godot 4**
1. Export as **PNG** (16-bit)
2. In Godot: Import as Texture
3. Apply to HeightMapShape3D
4. Adjust scale to match "Terrain Size"

---

## Troubleshooting

**Problem:** Mountains look too small in engine  
**Solution:** Increase "Terrain Size" value, regenerate

**Problem:** Character can't climb hills  
**Solution:** Use "Plains" instead of "Mountains"

**Problem:** Terrain feels too flat  
**Solution:** Switch from "Plains" to "Hills"

**Problem:** Can't paint higher  
**Solution:** Check status overlay - you hit max limit!

**Problem:** Need more extreme terrain  
**Solution:** Use "Custom" terrain type, adjust manually

---

## Feature Compatibility

| Feature | Compatible? |
|---------|-------------|
| Noise Generation | ✅ Yes |
| All Brush Types | ✅ Yes |
| Undo/Redo | ✅ Yes |
| Erosion Simulation | ✅ Yes |
| 3D Preview | ✅ Yes |
| Resolution Changes | ✅ Yes |
| Export (All Formats) | ✅ Yes |

---

## Version History

- **V0.5** - Foundation (noise, brushes, erosion)
- **V0.6** - Height clamping + visual feedback
- **V0.7** - Engine-aware presets ← YOU ARE HERE!
- **V0.8** - Import/Export pipeline (coming soon)
- **V0.9** - Advanced brushes (coming soon)
- **V1.0** - Production ready (goal)

---

## Getting Help

📖 **Full Guide:** See `ENGINE_AWARE_GUIDE.md`  
📊 **Visual Reference:** See `ENGINE_AWARE_VISUAL_REFERENCE.md`  
🎯 **Implementation Details:** See `ENGINE_AWARE_COMPLETE.md`  
📝 **TODO:** See `TODO.md` for roadmap  

---

## One-Line Summary

**"Select your engine and terrain type, paint, export - perfect scale every time!"**

---

*Quick Reference Card - V0.7*  
*Print this, keep it handy, create amazing game terrains! 🎨🎮*
