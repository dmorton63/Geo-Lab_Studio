#pragma once
#include <chrono>

enum class NoiseType {
    FBM = 0,
    Ridged = 1,
    Billowy = 2,
    Classic = 3
};

enum class BrushType {
    Raise = 0,
    Lower = 1,
    Smooth = 2,
    Flatten = 3
};

enum class TargetEngine {
    UnrealEngine = 0,
    Unity = 1,
    Godot = 2,
    Custom = 3
};

enum class TerrainType {
    Plains = 0,
    Hills = 1,
    Mountains = 2,
    Coastal = 3,
    Custom = 4
};

enum class EditorMode {
    Select = 0,   // View/navigate only
    Paint = 1,    // Paint mode (brush tools)
    Sculpt = 2,   // Future: Advanced sculpting
    Ramp = 3,     // Future: Ramp tool
    Smooth = 4    // Future: Smoothing mode
};

enum class StartingTemplate {
    PerlinNoise = 0,  // Procedural Perlin noise terrain
    FlatSculptable = 1,  // Flat plane for sculpting from scratch
    // Future templates:
    // Hills = 2,
    // Mountains = 3,
    // Coastal = 4
};

enum class MouseWheelMode {
    Radius = 0,      // [R] Adjust brush radius (5-100)
    Strength = 1,    // [S] Adjust brush strength (1-100)
    Feather = 2,     // [F] Adjust brush feather/softness (0.0-1.0)
    StartLevel = 3,  // [L] Adjust start level mask (0.0-1.0)
    MaxHeight = 4    // [H] Adjust max paint height (0.0-1.0)
};

struct LandscapeParameters {
    float waterLevel = 0.4f;
    float slopeThreshold = 0.01f;
    float noiseAmount = 0.0f;
    float heightmapFrequency = 6.28f;
    float erosionStrength = 0.0f;

    int noiseOctaves = 6;
    float noisePersistence = 0.5f;
    float noiseLacunarity = 2.0f;
    float noiseScale = 4.0f;
    NoiseType noiseType = NoiseType::FBM;

    int mapResolution = 256;
    int previewMode = 0;
    unsigned int seed = 42;

    bool autoUpdate = true;
    bool needsUpdate = true;

    bool view3D = false;
    float heightScale = 0.5f;

    // Editor mode system
    EditorMode currentMode = EditorMode::Select;

    bool paintMode = false;  // Legacy - will be replaced by currentMode == EditorMode::Paint
    BrushType brushType = BrushType::Raise;
    float brushRadius = 20.0f;
    float brushStrength = 50.0f;
    float flattenHeight = 0.5f;

    // Modal Mouse Wheel System (V0.8.0)
    MouseWheelMode mouseWheelMode = MouseWheelMode::Radius;  // Current parameter being adjusted
    float brushFeather = 0.5f;          // Edge softness (0.0 = hard edge, 1.0 = soft fade)
    float paintStartLevel = 0.0f;       // Height masking lower bound (only affect pixels below this)
    float maxPaintHeight = 1.0f;        // Paint ceiling (Raise) or floor (Lower)
    bool enableStartLevelMasking = false;  // Enable height masking feature
    bool enableMaxPaintHeight = false;     // Enable max height limiting
    bool showModeHUD = true;               // Show mode indicator overlay
    bool showAdjustmentOverlay = true;     // Show temporary value change feedback
    float mouseControlSensitivity = 1.0f;  // Scroll sensitivity multiplier

    // Height clamping parameters
    bool enableHeightClamping = true;
    float minHeight = 0.0f;
    float maxHeight = 1.0f;
    bool softClampMode = true;  // true = ease-in near limits, false = hard stop
    bool brushRespectsClamping = true;  // Per-brush override (future use)

    // Display options
    bool autoContrastDisplay = true;  // Auto-remap display range for visibility (doesn't modify data)

    // New Project Dialog
    bool showNewProjectDialog = false;
    StartingTemplate startingTemplate = StartingTemplate::PerlinNoise;

    // Engine-aware scaling
    TargetEngine targetEngine = TargetEngine::UnrealEngine;
    TerrainType terrainType = TerrainType::Hills;
    float characterHeight = 180.0f;  // cm (UE5 default)
    float terrainWorldSize = 1000.0f;  // meters (terrain physical size in engine)
    bool useEnginePresets = false;  // Enable engine-aware clamping
    bool normalizeTerrainToRange = false;  // Trigger to remap terrain to current min/max range

    bool erosionMode = false;
    int hydraulicIterations = 50000;
    int thermalIterations = 5;
    float rainAmount = 1.0f;
    float evaporation = 0.01f;
    float hydraulicStrength = 0.3f;
    float thermalStrength = 0.5f;
    float talusAngle = 0.05f;

    // Performance optimization - debouncing
    std::chrono::steady_clock::time_point lastChangeTime = std::chrono::steady_clock::now();
    float debounceDelayMs = 150.0f; // Wait 150ms after last change before regenerating

    void markChanged() {
        lastChangeTime = std::chrono::steady_clock::now();
        needsUpdate = true;
    }

    bool shouldUpdate() const {
        if (!needsUpdate || !autoUpdate) return false;
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastChangeTime).count();
        return elapsed >= debounceDelayMs;
    }
};
