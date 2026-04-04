#include "ErosionSimulator.h"
#include <algorithm>
#include <cmath>
#include <random>

ErosionSimulator::ErosionSimulator() {
}

void ErosionSimulator::applyHydraulicErosion(Image& heightmap, int iterations,
                                              float rainAmount, float evaporation,
                                              float erosionRate, float depositionRate) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distX(0.0f, static_cast<float>(heightmap.width() - 1));
    std::uniform_real_distribution<float> distY(0.0f, static_cast<float>(heightmap.height() - 1));

    for (int i = 0; i < iterations; ++i) {
        Droplet drop;
        drop.x = distX(gen);
        drop.y = distY(gen);
        drop.vx = 0.0f;
        drop.vy = 0.0f;
        drop.water = rainAmount;
        drop.sediment = 0.0f;
        drop.sedimentCapacity = 0.0f;

        simulateDroplet(heightmap, drop, erosionRate, depositionRate, evaporation, 64);
    }
}

void ErosionSimulator::applyThermalErosion(Image& heightmap, int iterations,
                                           float talusAngle, float erosionRate) {
    Image temp = heightmap;

    for (int iter = 0; iter < iterations; ++iter) {
        for (size_t y = 0; y < heightmap.height(); ++y) {
            for (size_t x = 0; x < heightmap.width(); ++x) {
                float centerHeight = temp(x, y);
                float totalDiff = 0.0f;
                float maxDiff = 0.0f;

                // Check 4 neighbors
                int dx[] = {-1, 1, 0, 0};
                int dy[] = {0, 0, -1, 1};

                for (int i = 0; i < 4; ++i) {
                    int nx = static_cast<int>(x) + dx[i];
                    int ny = static_cast<int>(y) + dy[i];

                    if (nx >= 0 && nx < static_cast<int>(heightmap.width()) &&
                        ny >= 0 && ny < static_cast<int>(heightmap.height())) {
                        
                        float neighborHeight = temp(nx, ny);
                        float diff = centerHeight - neighborHeight;

                        if (diff > talusAngle) {
                            totalDiff += diff;
                            maxDiff = std::max(maxDiff, diff);
                        }
                    }
                }

                if (totalDiff > 0.0f) {
                    float amount = erosionRate * maxDiff * 0.5f;
                    heightmap(x, y) = centerHeight - amount;

                    // Distribute to lower neighbors
                    for (int i = 0; i < 4; ++i) {
                        int nx = static_cast<int>(x) + dx[i];
                        int ny = static_cast<int>(y) + dy[i];

                        if (nx >= 0 && nx < static_cast<int>(heightmap.width()) &&
                            ny >= 0 && ny < static_cast<int>(heightmap.height())) {
                            
                            float neighborHeight = temp(nx, ny);
                            float diff = centerHeight - neighborHeight;

                            if (diff > talusAngle) {
                                float proportion = diff / totalDiff;
                                heightmap(nx, ny) = neighborHeight + amount * proportion;
                            }
                        }
                    }
                }
            }
        }

        temp = heightmap;
    }
}

void ErosionSimulator::applyErosion(Image& heightmap, int hydraulicIters, int thermalIters,
                                    float rainAmount, float evaporation,
                                    float hydraulicStrength, float thermalStrength,
                                    float talusAngle) {
    // Apply hydraulic erosion first (water carves valleys)
    if (hydraulicIters > 0) {
        applyHydraulicErosion(heightmap, hydraulicIters, rainAmount, evaporation,
                             hydraulicStrength, hydraulicStrength * 0.5f);
    }

    // Then thermal erosion (smooths steep slopes)
    if (thermalIters > 0) {
        applyThermalErosion(heightmap, thermalIters, talusAngle, thermalStrength);
    }
}

void ErosionSimulator::simulateDroplet(Image& heightmap, Droplet& drop,
                                       float erosionRate, float depositionRate,
                                       float evaporation, int maxSteps) {
    const float gravity = 4.0f;
    const float inertia = 0.05f;
    const float minSlope = 0.01f;

    for (int step = 0; step < maxSteps; ++step) {
        // Get current position and height
        int xi = static_cast<int>(drop.x);
        int yi = static_cast<int>(drop.y);

        if (xi < 0 || xi >= static_cast<int>(heightmap.width()) - 1 ||
            yi < 0 || yi >= static_cast<int>(heightmap.height()) - 1) {
            break;
        }

        float height = getHeightInterpolated(heightmap, drop.x, drop.y);

        // Calculate gradient (slope direction)
        float gradX = getHeightInterpolated(heightmap, drop.x + 1, drop.y) - height;
        float gradY = getHeightInterpolated(heightmap, drop.x, drop.y + 1) - height;

        // Update velocity
        drop.vx = drop.vx * inertia - gradX * (1.0f - inertia);
        drop.vy = drop.vy * inertia - gradY * (1.0f - inertia);

        // Normalize velocity
        float len = std::sqrt(drop.vx * drop.vx + drop.vy * drop.vy);
        if (len > 0.0f) {
            drop.vx /= len;
            drop.vy /= len;
        }

        // Move droplet
        float newX = drop.x + drop.vx;
        float newY = drop.y + drop.vy;

        if (newX < 0 || newX >= heightmap.width() - 1 ||
            newY < 0 || newY >= heightmap.height() - 1) {
            break;
        }

        float newHeight = getHeightInterpolated(heightmap, newX, newY);
        float heightDiff = newHeight - height;

        // Calculate sediment capacity
        drop.sedimentCapacity = std::max(-heightDiff, minSlope) * drop.water * gravity;

        // Erode or deposit
        if (drop.sediment > drop.sedimentCapacity || heightDiff > 0) {
            // Deposit sediment
            float amountToDeposit = (heightDiff > 0) ? 
                std::min(heightDiff, drop.sediment) : 
                (drop.sediment - drop.sedimentCapacity) * depositionRate;
            
            drop.sediment -= amountToDeposit;
            depositSediment(heightmap, drop.x, drop.y, amountToDeposit);
        } else {
            // Erode terrain
            float amountToErode = std::min((drop.sedimentCapacity - drop.sediment) * erosionRate, -heightDiff);
            erodeTerrain(heightmap, drop.x, drop.y, amountToErode);
            drop.sediment += amountToErode;
        }

        // Update position and evaporate water
        drop.x = newX;
        drop.y = newY;
        drop.water *= (1.0f - evaporation);

        if (drop.water < 0.01f) break;
    }

    // Deposit remaining sediment
    if (drop.sediment > 0.0f) {
        depositSediment(heightmap, drop.x, drop.y, drop.sediment);
    }
}

float ErosionSimulator::getHeightInterpolated(const Image& heightmap, float x, float y) const {
    int xi = static_cast<int>(std::floor(x));
    int yi = static_cast<int>(std::floor(y));

    xi = std::clamp(xi, 0, static_cast<int>(heightmap.width()) - 2);
    yi = std::clamp(yi, 0, static_cast<int>(heightmap.height()) - 2);

    float fx = x - xi;
    float fy = y - yi;

    float h00 = heightmap(xi, yi);
    float h10 = heightmap(xi + 1, yi);
    float h01 = heightmap(xi, yi + 1);
    float h11 = heightmap(xi + 1, yi + 1);

    float h0 = h00 * (1.0f - fx) + h10 * fx;
    float h1 = h01 * (1.0f - fx) + h11 * fx;

    return h0 * (1.0f - fy) + h1 * fy;
}

void ErosionSimulator::depositSediment(Image& heightmap, float x, float y, float amount) {
    int xi = static_cast<int>(std::floor(x));
    int yi = static_cast<int>(std::floor(y));

    xi = std::clamp(xi, 0, static_cast<int>(heightmap.width()) - 2);
    yi = std::clamp(yi, 0, static_cast<int>(heightmap.height()) - 2);

    float fx = x - xi;
    float fy = y - yi;

    heightmap(xi, yi) += amount * (1.0f - fx) * (1.0f - fy);
    heightmap(xi + 1, yi) += amount * fx * (1.0f - fy);
    heightmap(xi, yi + 1) += amount * (1.0f - fx) * fy;
    heightmap(xi + 1, yi + 1) += amount * fx * fy;
}

void ErosionSimulator::erodeTerrain(Image& heightmap, float x, float y, float amount) {
    depositSediment(heightmap, x, y, -amount);
}
