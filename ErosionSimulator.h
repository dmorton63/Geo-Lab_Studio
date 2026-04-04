#pragma once
#include "Image.h"

class ErosionSimulator {
public:
    ErosionSimulator();

    // Hydraulic erosion - simulates water flow and sediment transport
    void applyHydraulicErosion(Image& heightmap, int iterations, 
                               float rainAmount, float evaporation, 
                               float erosionRate, float depositionRate);

    // Thermal erosion - simulates material collapse on steep slopes
    void applyThermalErosion(Image& heightmap, int iterations, 
                            float talusAngle, float erosionRate);

    // Combined erosion for best results
    void applyErosion(Image& heightmap, int hydraulicIters, int thermalIters,
                     float rainAmount, float evaporation,
                     float hydraulicStrength, float thermalStrength,
                     float talusAngle);

private:
    struct Droplet {
        float x, y;           // Position
        float vx, vy;         // Velocity
        float water;          // Water amount
        float sediment;       // Sediment carried
        float sedimentCapacity; // Max sediment
    };

    void simulateDroplet(Image& heightmap, Droplet& drop, 
                        float erosionRate, float depositionRate, 
                        float evaporation, int maxSteps);
    
    float getHeightInterpolated(const Image& heightmap, float x, float y) const;
    void depositSediment(Image& heightmap, float x, float y, float amount);
    void erodeTerrain(Image& heightmap, float x, float y, float amount);
};
