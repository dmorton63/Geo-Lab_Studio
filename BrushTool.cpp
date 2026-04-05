#include "BrushTool.h"
#include <algorithm>
#include <cmath>

BrushTool::BrushTool()
    : flattenHeight(0.5f)
{
}

void BrushTool::applyBrush(Image& heightmap, int centerX, int centerY, BrushType type,
                           float radius, float strength, const LandscapeParameters& params) {
    switch (type) {
        case BrushType::Raise:
            applyRaise(heightmap, centerX, centerY, radius, strength, params);
            break;
        case BrushType::Lower:
            applyLower(heightmap, centerX, centerY, radius, strength, params);
            break;
        case BrushType::Smooth:
            applySmooth(heightmap, centerX, centerY, radius, strength, params);
            break;
        case BrushType::Flatten:
            applyFlatten(heightmap, centerX, centerY, radius, strength, params);
            break;
    }
}

float BrushTool::calculateBrushWeight(float distance, float radius) const {
    if (distance >= radius) return 0.0f;

    // Smooth falloff using cosine curve
    float normalizedDist = distance / radius;
    return (std::cos(normalizedDist * 3.14159f) + 1.0f) * 0.5f;
}

float BrushTool::applyHeightClamping(float newHeight, float currentHeight, const LandscapeParameters& params) const {
    if (!params.enableHeightClamping || !params.brushRespectsClamping) {
        return newHeight;
    }

    // CRITICAL: Never move terrain in opposite direction of brush intent!
    // If raising (newHeight > current), never lower below current
    // If lowering (newHeight < current), never raise above current

    bool isRaising = newHeight > currentHeight;
    bool isLowering = newHeight < currentHeight;

    // Hard clamp mode - immediate stop at limits, but respect brush direction
    if (!params.softClampMode) {
        float clamped = std::clamp(newHeight, params.minHeight, params.maxHeight);

        // Prevent opposite-direction movement
        if (isRaising && clamped < currentHeight) {
            return currentHeight; // Don't lower when trying to raise
        }
        if (isLowering && clamped > currentHeight) {
            return currentHeight; // Don't raise when trying to lower
        }

        return clamped;
    }

    // Soft clamp mode - ease-in near limits to preserve natural feel
    const float range = params.maxHeight - params.minHeight;

    // Safety check: if range is too small, use hard clamp logic
    if (range < 0.001f) {
        float clamped = std::clamp(newHeight, params.minHeight, params.maxHeight);

        // Prevent opposite-direction movement
        if (isRaising && clamped < currentHeight) {
            return currentHeight;
        }
        if (isLowering && clamped > currentHeight) {
            return currentHeight;
        }

        return clamped;
    }

    const float softZone = 0.05f; // 5% of range on each side for easing
    const float softThreshold = std::max(range * softZone, 0.001f); // Minimum threshold

    // Check upper limit (only when raising)
    if (newHeight > params.maxHeight) {
        // If lowering, allow it even if above max
        if (isLowering) {
            return newHeight; // Allow lowering from above max
        }
        // If raising, stop at max or current (whichever is higher)
        return std::max(currentHeight, params.maxHeight);
    }
    else if (newHeight > params.maxHeight - softThreshold && isRaising) {
        // Ease-in near upper limit (only when raising)
        float distFromLimit = params.maxHeight - newHeight;
        float easeFactor = distFromLimit / softThreshold; // 0.0 at limit, 1.0 at threshold
        float delta = newHeight - currentHeight;
        return currentHeight + delta * easeFactor;
    }

    // Check lower limit (only when lowering)
    if (newHeight < params.minHeight) {
        // If raising, allow it even if below min
        if (isRaising) {
            return newHeight; // Allow raising from below min
        }
        // If lowering, stop at min or current (whichever is lower)
        return std::min(currentHeight, params.minHeight);
    }
    else if (newHeight < params.minHeight + softThreshold && isLowering) {
        // Ease-in near lower limit (only when lowering)
        float distFromLimit = newHeight - params.minHeight;
        float easeFactor = distFromLimit / softThreshold;
        float delta = newHeight - currentHeight;
        return currentHeight + delta * easeFactor;
    }

    return newHeight;
}

float BrushTool::getHeightAtCursor(const Image& heightmap, int x, int y) {
    if (x < 0 || x >= static_cast<int>(heightmap.width()) || 
        y < 0 || y >= static_cast<int>(heightmap.height())) {
        return 0.0f;
    }
    return heightmap(x, y);
}

void BrushTool::applyRaise(Image& heightmap, int centerX, int centerY, float radius, float strength, const LandscapeParameters& params) {
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
                float newHeight = currentHeight + delta;
                heightmap(x, y) = applyHeightClamping(newHeight, currentHeight, params);
            }
        }
    }
}

void BrushTool::applyLower(Image& heightmap, int centerX, int centerY, float radius, float strength, const LandscapeParameters& params) {
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
                float newHeight = currentHeight - delta;
                heightmap(x, y) = applyHeightClamping(newHeight, currentHeight, params);
            }
        }
    }
}

void BrushTool::applySmooth(Image& heightmap, int centerX, int centerY, float radius, float strength, const LandscapeParameters& params) {
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
                float newHeight = currentHeight + (average - currentHeight) * smoothStrength;
                heightmap(x, y) = applyHeightClamping(newHeight, currentHeight, params);
            }
        }
    }
}

void BrushTool::applyFlatten(Image& heightmap, int centerX, int centerY, float radius, float strength, const LandscapeParameters& params) {
    int radiusInt = static_cast<int>(std::ceil(radius));

    // Clamp flatten target height to valid range
    float targetHeight = flattenHeight;
    if (params.enableHeightClamping) {
        targetHeight = std::clamp(flattenHeight, params.minHeight, params.maxHeight);
    }

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
                float newHeight = currentHeight + (targetHeight - currentHeight) * flattenStrength;
                heightmap(x, y) = applyHeightClamping(newHeight, currentHeight, params);
            }
        }
    }
}
