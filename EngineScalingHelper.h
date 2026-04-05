#pragma once
#include "LandscapeParameters.h"
#include <string>
#include <utility>

// Engine-aware terrain scaling helper
// Converts real-world dimensions (meters) to normalized heightmap values (0.0-1.0)
class EngineScalingHelper {
public:
    // Engine character specifications
    struct EngineSpec {
        std::string name;
        float characterHeight;  // cm
        float capsuleRadius;    // cm
        float walkableSlopeAngle;  // degrees
        std::string notes;
    };

    // Terrain type height ranges (relative to character height)
    struct TerrainRange {
        std::string name;
        float minHeightMeters;  // Minimum elevation variation
        float maxHeightMeters;  // Maximum elevation variation
        std::string description;
    };

    // Get engine specifications
    static EngineSpec getEngineSpec(TargetEngine engine) {
        switch (engine) {
            case TargetEngine::UnrealEngine:
                return { "Unreal Engine 5", 180.0f, 42.0f, 45.0f, 
                        "Default Third Person Character capsule" };
            
            case TargetEngine::Unity:
                return { "Unity", 200.0f, 50.0f, 45.0f, 
                        "Default Character Controller" };
            
            case TargetEngine::Godot:
                return { "Godot 4", 180.0f, 40.0f, 46.0f, 
                        "Default CharacterBody3D capsule" };
            
            case TargetEngine::Custom:
            default:
                return { "Custom", 180.0f, 40.0f, 45.0f, 
                        "User-defined specifications" };
        }
    }

    // Get terrain type range relative to character height
    static TerrainRange getTerrainRange(TerrainType type, float characterHeightCm) {
        float charHeightM = characterHeightCm / 100.0f;
        
        switch (type) {
            case TerrainType::Plains:
                return { "Plains", 
                        0.0f, 
                        charHeightM * 1.2f,  // Subtle variation (0-2m for 180cm character)
                        "Gentle rolling terrain, mostly flat" };
            
            case TerrainType::Hills:
                return { "Hills", 
                        0.0f, 
                        charHeightM * 8.0f,  // Moderate hills (0-14m for 180cm character)
                        "Rolling hills with noticeable elevation changes" };
            
            case TerrainType::Mountains:
                return { "Mountains", 
                        0.0f, 
                        charHeightM * 55.0f,  // Dramatic peaks (0-100m for 180cm character)
                        "Steep mountains with dramatic peaks" };
            
            case TerrainType::Coastal:
                return { "Coastal", 
                        -charHeightM * 2.0f,  // Below water level
                        charHeightM * 3.0f,   // Beach + gentle cliffs
                        "Beach with ocean floor and coastal cliffs" };
            
            case TerrainType::Custom:
            default:
                return { "Custom", 0.0f, charHeightM * 10.0f, 
                        "User-defined height range" };
        }
    }

    // Convert real-world meters to normalized heightmap value (0.0-1.0)
    // terrainWorldSize = physical size of terrain in engine (e.g., 1000m x 1000m)
    static float metersToNormalized(float meters, float terrainWorldSize) {
        // Terrain height range is a fraction of the terrain's horizontal size
        // For a 1000m x 1000m terrain, 100m elevation = 0.1 (10% of size)
        return meters / terrainWorldSize;
    }

    // Convert normalized heightmap value to real-world meters
    static float normalizedToMeters(float normalized, float terrainWorldSize) {
        return normalized * terrainWorldSize;
    }

    // Calculate natural clamping range based on engine and terrain type
    static std::pair<float, float> calculateClampRange(
        const LandscapeParameters& params
    ) {
        // Get terrain range in meters
        TerrainRange range = getTerrainRange(params.terrainType, params.characterHeight);
        
        // Convert to normalized values (0.0-1.0)
        float minNormalized = metersToNormalized(range.minHeightMeters, params.terrainWorldSize);
        float maxNormalized = metersToNormalized(range.maxHeightMeters, params.terrainWorldSize);
        
        // Clamp to valid heightmap range
        minNormalized = std::max(0.0f, std::min(1.0f, minNormalized));
        maxNormalized = std::max(0.0f, std::min(1.0f, maxNormalized));
        
        return { minNormalized, maxNormalized };
    }

    // Get descriptive text for UI display
    static std::string getHeightRangeDescription(
        TargetEngine engine, 
        TerrainType type, 
        float characterHeightCm,
        float terrainWorldSize
    ) {
        EngineSpec spec = getEngineSpec(engine);
        TerrainRange range = getTerrainRange(type, characterHeightCm);
        
        float charHeightM = characterHeightCm / 100.0f;
        
        char buffer[512];
        snprintf(buffer, sizeof(buffer),
            "%s\n"
            "Character: %.0fcm (%.2fm)\n"
            "Terrain Size: %.0fm x %.0fm\n"
            "Height Range: %.1fm to %.1fm\n"
            "%s",
            spec.name.c_str(),
            characterHeightCm,
            charHeightM,
            terrainWorldSize,
            terrainWorldSize,
            range.minHeightMeters,
            range.maxHeightMeters,
            range.description.c_str()
        );
        
        return std::string(buffer);
    }
};
