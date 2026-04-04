# Erosion Simulation Guide

## Landscape Designer v0.5 - Erosion Simulation Feature

### Overview
Erosion simulation adds realistic weathering effects to your terrain, making it look naturally aged. Two types of erosion work together to create believable landscapes: hydraulic (water) and thermal (gravity).

### What is Erosion?

#### 🌊 Hydraulic Erosion
Simulates water droplets falling on terrain, flowing downhill, picking up sediment, and depositing it elsewhere.

**Effects:**
- Carves valleys and gorges
- Creates river networks
- Deposits sediment in low areas
- Forms alluvial fans and deltas
- Softens sharp peaks

**How it works:**
1. Thousands of water droplets spawn randomly
2. Each droplet flows downhill following gravity
3. Fast-moving water erodes terrain (picks up sediment)
4. Slow-moving water deposits sediment
5. Water evaporates over time

#### ⛰️ Thermal Erosion
Simulates material sliding down steep slopes due to gravity.

**Effects:**
- Smooths overly steep cliffs
- Creates talus slopes (debris piles at cliff bases)
- Redistributes material from peaks to valleys
- Makes terrain more geologically plausible
- Reduces unrealistic sharp angles

**How it works:**
1. Checks each pixel's height vs neighbors
2. If slope exceeds "talus angle" (angle of repose)
3. Material slides down to neighbors
4. Iterates to reach equilibrium

### Erosion Parameters

#### Hydraulic Erosion

**Droplets** (0 - 200,000)
- Number of water droplets to simulate
- More droplets = more erosion detail
- 50,000 = moderate erosion
- 100,000+ = heavy erosion
- Logarithmic slider for easy adjustment
- ⚠️ Higher values take longer to compute

**Rain Amount** (0.1 - 5.0)
- Initial water volume per droplet
- Higher = more erosive power
- 1.0 = default realistic amount
- 3.0+ = flash flood effect

**Evaporation** (0.001 - 0.1)
- Water loss rate per step
- Lower = droplets flow farther
- 0.01 = default balanced value
- 0.001 = creates long river systems

**Erosion Strength** (0.1 - 1.0)
- How quickly water erodes terrain
- Higher = faster, more dramatic carving
- 0.3 = default subtle erosion
- 0.7+ = aggressive canyon formation

#### Thermal Erosion

**Iterations** (0 - 20)
- Number of passes over terrain
- More = smoother slopes
- 5 = default balanced smoothing
- 10+ = very smooth, gentle slopes

**Talus Angle** (0.01 - 0.2)
- Maximum stable slope height difference
- Lower = smoother terrain (less steep allowed)
- 0.05 = default realistic value
- 0.01 = very smooth, gradual slopes
- 0.2 = allows steep cliffs

**Erosion Strength** (0.1 - 1.0)
- Speed of material redistribution
- Higher = faster smoothing
- 0.5 = default balanced value
- 0.9 = rapid slope reduction

### Apply Buttons

#### "Apply Hydraulic Only"
- Runs only water erosion
- Best for: Creating river valleys, canyons
- Fast computation
- Preserves sharp peaks

#### "Apply Thermal Only"  
- Runs only gravity erosion
- Best for: Smoothing painted terrain, softening noise
- Very fast computation
- Creates gentle slopes

#### "Apply Both (Recommended)" ⭐
- Runs hydraulic first, then thermal
- Best for: Realistic natural terrain
- Hydraulic carves valleys
- Thermal smooths unrealistic slopes
- Most believable results

### Usage Workflow

#### Basic Erosion
1. Generate terrain with Perlin noise
2. Open "Erosion Simulation" section
3. Leave default settings
4. Click "Apply Both (Recommended)"
5. Wait a few seconds
6. View results

#### Custom Erosion

**For Deep Canyons:**
1. Hydraulic Droplets: 100,000
2. Rain Amount: 2.0
3. Erosion Strength: 0.7
4. Thermal Iterations: 3
5. Apply Both

**For Gentle Hills:**
1. Hydraulic Droplets: 20,000
2. Erosion Strength: 0.2
3. Thermal Iterations: 10
4. Talus Angle: 0.02
5. Apply Both

**For River Networks:**
1. Hydraulic Droplets: 150,000
2. Evaporation: 0.005
3. Erosion Strength: 0.4
4. Apply Hydraulic Only

### Tips & Best Practices

#### Before Eroding
- ✅ Export original terrain as backup
- ✅ Generate at 256x256 or higher for best detail
- ✅ Use FBM or Ridged noise as starting point
- ✅ Disable auto-update to prevent regeneration

#### During Erosion
- ⏱️ Be patient - 100K+ droplets take 10-30 seconds
- 👁️ Watch console for "Applying erosion..." message
- 🔄 Can apply erosion multiple times
- 📊 Switch to Slope view to see erosion effects

#### After Eroding
- 🔍 Check Slope preview to verify natural angles
- 🌊 Check Water Mask to see drainage patterns
- 🎨 Use Paint Mode to touch up areas if needed
- 💾 Export eroded terrain before further changes

### Combining with Other Features

#### Erosion + Paint Mode
1. Generate terrain
2. Apply erosion
3. Enable Paint Mode
4. Touch up specific areas
5. Apply light thermal erosion to blend

#### Erosion + 3D View
1. Apply erosion
2. Enable 3D View
3. Rotate to check valleys and peaks
4. Exit 3D, adjust parameters
5. Apply more erosion if needed

#### Progressive Erosion
1. Apply 25,000 droplets
2. Review results
3. Apply 25,000 more
4. Repeat until satisfied
5. Finish with thermal smoothing

### Performance Considerations

**Fast (< 1 second):**
- Thermal erosion only
- < 10,000 hydraulic droplets

**Moderate (1-5 seconds):**
- 10,000 - 50,000 droplets
- Default settings

**Slow (5-30 seconds):**
- 50,000 - 200,000 droplets
- 512x512 resolution

**Very Slow (30+ seconds):**
- 200,000+ droplets
- 512x512 with high iterations

### Technical Details

#### Hydraulic Algorithm
- Based on "Fast Hydraulic Erosion Simulation"
- Droplet lifecycle: spawn → flow → erode → deposit
- Uses bilinear interpolation for smooth results
- Sediment capacity based on velocity and slope
- Inertia parameter prevents unrealistic zigzag paths

#### Thermal Algorithm
- Iterative slope-based redistribution
- 4-neighbor analysis (up, down, left, right)
- Material flows to all downhill neighbors proportionally
- Reaches equilibrium after enough iterations

#### Erosion Effects on Data
- **Height**: Direct modification (carves and fills)
- **Slope**: Recalculated from new heights
- **Curvature**: Recalculated from new slopes
- **Biomes**: Updated based on new heights
- **Water**: New drainage patterns form

### Troubleshooting

**Erosion has no visible effect:**
- Increase droplet count
- Increase erosion strength
- Check that erosionMode triggers (console message)

**Terrain becomes too flat:**
- Reduce thermal iterations
- Increase talus angle
- Lower thermal erosion strength

**Erosion takes too long:**
- Reduce droplet count
- Lower map resolution
- Use thermal only for quick smoothing

**Unrealistic spikes remain:**
- Apply thermal erosion
- Increase thermal iterations
- Or use Smooth brush in Paint Mode

**Want undo:**
- Export before eroding
- Regenerate terrain to start over
- No undo in erosion yet (planned)

### Example Settings

#### Grand Canyon Style
```
Hydraulic Droplets: 150,000
Rain Amount: 3.0
Evaporation: 0.008
Erosion Strength: 0.8
Thermal Iterations: 2
Talus Angle: 0.1
```

#### Smooth Rolling Hills
```
Hydraulic Droplets: 30,000
Erosion Strength: 0.2
Thermal Iterations: 15
Talus Angle: 0.02
```

#### Mountain Valleys
```
Hydraulic Droplets: 80,000
Rain Amount: 1.5
Erosion Strength: 0.5
Thermal Iterations: 5
Talus Angle: 0.05
```

#### Dramatic Badlands
```
Hydraulic Droplets: 200,000
Rain Amount: 4.0
Erosion Strength: 0.9
Thermal Iterations: 1
Talus Angle: 0.15
```

### Scientific Accuracy

This erosion simulation is **game-oriented**, not scientifically accurate:
- ✅ Creates visually believable terrain
- ✅ Mimics real erosion patterns
- ✅ Fast enough for interactive use
- ❌ Not suitable for geological research
- ❌ Simplified physics
- ❌ No chemical weathering, freeze-thaw, etc.

For realistic results in games/art, these simplifications work great!

---

**Congratulations! You now have a complete terrain generation suite!** 🎉

Experiment with different combinations of noise, painting, and erosion to create unique landscapes!
