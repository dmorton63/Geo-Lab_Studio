# Paint Mode Guide

## Landscape Designer v0.4 - Paint Mode Feature

### Overview
Paint Mode allows you to manually sculpt and refine your terrain using brush tools. You can raise, lower, smooth, or flatten areas with precise control.

### Enabling Paint Mode
1. Open the "Paint Mode" section in the UI
2. Check "Enable Paint Mode"
3. Note: Auto-update is automatically disabled in paint mode
4. Note: 3D view is disabled while painting (2D only)

### Brush Types

#### 🔺 Raise
- Increases terrain height
- Perfect for creating hills, mountains, or elevated areas
- Effect is gradual with smooth falloff

#### 🔻 Lower
- Decreases terrain height
- Great for valleys, riverbeds, or depressions
- Smooth falloff prevents harsh edges

#### 🌊 Smooth
- Averages heights in the brush area
- Excellent for softening rough terrain
- Removes sharp transitions and noise
- Uses 3x3 neighborhood averaging

#### ⬜ Flatten
- Pushes terrain toward a target height
- Creates plateaus, terraces, or flat areas
- Set target height with "Flatten Height" slider
- "Sample" button (coming soon) will pick height from terrain

### Brush Parameters

#### Brush Radius (5 - 100 pixels)
- Controls the size of the affected area
- Larger radius = broader strokes
- Smaller radius = precise detailing
- Visual cursor shows exact brush size

#### Brush Strength (1 - 100%)
- Controls how quickly the effect is applied
- Higher strength = faster changes
- Lower strength = subtle adjustments
- Allows for fine control with multiple passes

#### Flatten Height (0.0 - 1.0)
- Only visible when Flatten brush is selected
- Target height for flattening operation
- 0.0 = lowest point, 1.0 = highest point
- Use "Sample" to pick height from terrain (coming soon)

### Controls
- **Paint**: Left-click and drag to apply brush
- **Cursor**: White circle shows brush radius and position
- **Undo**: Ctrl+Z (coming soon in next update)

### Workflow Tips

1. **Generate Base Terrain**
   - Use Perlin noise to create initial landscape
   - Disable auto-update before painting

2. **Roughing In**
   - Use large Raise/Lower brushes for major features
   - Set high strength for quick work

3. **Refining Details**
   - Decrease brush radius for precision work
   - Lower strength for subtle adjustments
   - Use Smooth to blend transitions

4. **Creating Plateaus**
   - Set desired Flatten Height
   - Use Flatten brush on target area
   - Smooth edges if needed

5. **Final Polish**
   - Use Smooth brush on entire terrain
   - Touch up any artifacts
   - Add final details with small Raise/Lower

### Technical Details

#### Brush Falloff
- Uses cosine curve for smooth transitions
- Weight = (cos(distance/radius * π) + 1) / 2
- Center has full strength, edge has zero strength
- No harsh boundaries or visible brush strokes

#### Brush Application
- **Raise/Lower**: Direct height modification
  - delta = strength * weight * 0.01
  - height = clamp(height ± delta, 0, 1)

- **Smooth**: Neighborhood averaging
  - Samples 3x3 area around each pixel
  - Blends toward average with strength factor
  - Preserves general shape while softening

- **Flatten**: Interpolation to target
  - Moves height toward flatten target
  - Rate controlled by strength * weight
  - Gradual approach to target height

#### Performance
- Real-time updates while painting
- Design pipeline (slope, curvature, biome) recalculated
- Optimized for brush sizes up to 100 pixels
- Smooth at 60 FPS on 256x256 heightmaps

### Paint Mode Limitations
- **2D Only**: 3D view disabled while painting
  - Switch back to normal mode to view in 3D
  - Changes persist when re-enabling 3D

- **No Undo Yet**: Coming in next update
  - Be careful with destructive edits
  - Consider exporting before major changes

- **Auto-Update Disabled**: Manual regeneration only
  - Parameter changes don't apply while painting
  - Exit paint mode to regenerate from scratch

### Exiting Paint Mode
1. Click "Exit Paint Mode" button
2. Auto-update re-enables
3. Can regenerate terrain or continue with painted version
4. Painted changes remain until regeneration

### Combining with Other Features

#### Export Painted Terrain
- Paint mode works with all export formats
- Save your custom terrain as PNG or RAW
- Export biome/slope/curvature of painted terrain

#### 3D Visualization
- Exit paint mode first
- Enable 3D view to see results
- Rotate to check from all angles
- Return to paint mode for adjustments

#### Preview Modes
- All preview modes work while painting
- View height, slope, curvature as you paint
- Biome colors update in real-time
- Water mask reflects height changes

### Best Practices

1. **Save Often**: Export periodically to preserve work
2. **Work in Layers**: Rough shapes first, details last
3. **Use Low Strength**: Multiple light passes > one heavy stroke
4. **Preview Slopes**: Switch to slope view to check steepness
5. **Test in 3D**: Periodically check results in 3D view

### Example Workflows

#### Creating a Volcano
1. Generate FBM terrain with low octaves
2. Enable Paint Mode with Raise brush
3. Large radius (80), high strength (80)
4. Build central cone with spiral strokes
5. Switch to Lower, medium radius (40)
6. Create crater at peak
7. Smooth brush to refine slopes
8. Exit and view in 3D

#### Carving a River Valley
1. Start with Ridged noise for mountains
2. Paint Mode with Lower brush
3. Medium radius (50), medium strength (50)
4. Draw winding path from high to low
5. Smooth brush along edges
6. Flatten brush for river bed
7. Check water mask to see water flow

#### Making Terraced Fields
1. Generate gentle slopes with FBM
2. Paint Mode with Flatten brush
3. Set flatten height to 0.3
4. Paint first terrace level
5. Increase to 0.4, paint next level
6. Continue stepping up
7. Smooth between levels for natural look

Enjoy sculpting your perfect landscape! 🎨
