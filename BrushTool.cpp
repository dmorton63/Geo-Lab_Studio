#include "BrushTool.h"
#include <algorithm>
#include <cmath>

BrushTool::BrushTool()
    : flattenHeight(0.5f)
{
}

void BrushTool::applyBrush(Image& heightmap, int centerX, int centerY, BrushType type,
                           float radius, float strength) {
    switch (type) {
        case BrushType::Raise:
            applyRaise(heightmap, centerX, centerY, radius, strength);
            break;
        case BrushType::Lower:
            applyLower(heightmap, centerX, centerY, radius, strength);
            break;
        case BrushType::Smooth:
            applySmooth(heightmap, centerX, centerY, radius, strength);
            break;
        case BrushType::Flatten:
            applyFlatten(heightmap, centerX, centerY, radius, strength);
            break;
    }
}

float BrushTool::calculateBrushWeight(float distance, float radius) const {
    if (distance >= radius) return 0.0f;
    
    // Smooth falloff using cosine curve
    float normalizedDist = distance / radius;
    return (std::cos(normalizedDist * 3.14159f) + 1.0f) * 0.5f;
}

void BrushTool::applyRaise(Image& heightmap, int centerX, int centerY, float radius, float strength) {
    int radiusInt = static_cast<int>(std::ceil(radius));

    for (int y = std::max(0, centerY - radiusInt); y <= std::min(static_cast<int>(heightmap.height()) - 1, centerY + radiusInt); ++y) {
        for (int x = std::max(0, centerX - radiusInt); x <= std::min(static_cast<int>(heightmap.width()) - 1, centerX + radiusInt); ++x) {
            float dx = static_cast<float>(x - centerX);
            float dy = static_cast<float>(y - centerY);
            float distance = std::sqrt(dx * dx + dy * dy);

            float weight = calculateBrushWeight(distance, radius);
            if (weight > 0.0f) {
                float currentHeight = heightmap(x, y);
                // Strength is 0-100, convert to 0.0-0.01 for incremental raise per frame
                float delta = (strength / 100.0f) * weight * 0.01f;
                heightmap(x, y) = std::clamp(currentHeight + delta, 0.0f, 1.0f);
            }
        }
    }
}

void BrushTool::applyLower(Image& heightmap, int centerX, int centerY, float radius, float strength) {
    int radiusInt = static_cast<int>(std::ceil(radius));

    for (int y = std::max(0, centerY - radiusInt); y <= std::min(static_cast<int>(heightmap.height()) - 1, centerY + radiusInt); ++y) {
        for (int x = std::max(0, centerX - radiusInt); x <= std::min(static_cast<int>(heightmap.width()) - 1, centerX + radiusInt); ++x) {
            float dx = static_cast<float>(x - centerX);
            float dy = static_cast<float>(y - centerY);
            float distance = std::sqrt(dx * dx + dy * dy);

            float weight = calculateBrushWeight(distance, radius);
            if (weight > 0.0f) {
                float currentHeight = heightmap(x, y);
                // Strength is 0-100, convert to 0.0-0.01 for incremental lower per frame
                float delta = (strength / 100.0f) * weight * 0.01f;
                heightmap(x, y) = std::clamp(currentHeight - delta, 0.0f, 1.0f);
            }
        }
    }
}

void BrushTool::applySmooth(Image& heightmap, int centerX, int centerY, float radius, float strength) {
    int radiusInt = static_cast<int>(std::ceil(radius));
    Image temp = heightmap;
    
    for (int y = std::max(0, centerY - radiusInt); y <= std::min(static_cast<int>(heightmap.height()) - 1, centerY + radiusInt); ++y) {
        for (int x = std::max(0, centerX - radiusInt); x <= std::min(static_cast<int>(heightmap.width()) - 1, centerX + radiusInt); ++x) {
            float dx = static_cast<float>(x - centerX);
            float dy = static_cast<float>(y - centerY);
            float distance = std::sqrt(dx * dx + dy * dy);
            
            float weight = calculateBrushWeight(distance, radius);
            if (weight > 0.0f) {
                // Calculate average height in 3x3 neighborhood
                float sum = 0.0f;
                int count = 0;
                
                for (int ny = std::max(0, y - 1); ny <= std::min(static_cast<int>(temp.height()) - 1, y + 1); ++ny) {
                    for (int nx = std::max(0, x - 1); nx <= std::min(static_cast<int>(temp.width()) - 1, x + 1); ++nx) {
                        sum += temp(nx, ny);
                        count++;
                    }
                }
                
                float average = sum / static_cast<float>(count);
                float currentHeight = heightmap(x, y);
                // Normalize strength from 0-100 to 0.0-1.0, then scale for smooth blending
                float smoothStrength = (strength / 100.0f) * weight * 0.1f;
                heightmap(x, y) = currentHeight + (average - currentHeight) * smoothStrength;
            }
        }
    }
}

void BrushTool::applyFlatten(Image& heightmap, int centerX, int centerY, float radius, float strength) {
    int radiusInt = static_cast<int>(std::ceil(radius));
    
    for (int y = std::max(0, centerY - radiusInt); y <= std::min(static_cast<int>(heightmap.height()) - 1, centerY + radiusInt); ++y) {
        for (int x = std::max(0, centerX - radiusInt); x <= std::min(static_cast<int>(heightmap.width()) - 1, centerX + radiusInt); ++x) {
            float dx = static_cast<float>(x - centerX);
            float dy = static_cast<float>(y - centerY);
            float distance = std::sqrt(dx * dx + dy * dy);
            
            float weight = calculateBrushWeight(distance, radius);
            if (weight > 0.0f) {
                float currentHeight = heightmap(x, y);
                // Normalize strength from 0-100 to 0.0-1.0, then scale for flatten blending
                float flattenStrength = (strength / 100.0f) * weight * 0.1f;
                heightmap(x, y) = currentHeight + (flattenHeight - currentHeight) * flattenStrength;
            }
        }
    }
}
