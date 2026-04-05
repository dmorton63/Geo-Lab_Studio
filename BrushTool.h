#pragma once
#include "Image.h"
#include "LandscapeParameters.h"
#include <vector>

class BrushTool {
public:
    BrushTool();

    void applyBrush(Image& heightmap, int centerX, int centerY, BrushType type, 
                    float radius, float strength, const LandscapeParameters& params);

    void setFlattenHeight(float height) { flattenHeight = height; }
    float getFlattenHeight() const { return flattenHeight; }

    // Get height at cursor position for visual feedback
    static float getHeightAtCursor(const Image& heightmap, int x, int y);

private:
    float flattenHeight;

    float calculateBrushWeight(float distance, float radius) const;
    float applyHeightClamping(float newHeight, float currentHeight, const LandscapeParameters& params) const;

    void applyRaise(Image& heightmap, int centerX, int centerY, float radius, float strength, const LandscapeParameters& params);
    void applyLower(Image& heightmap, int centerX, int centerY, float radius, float strength, const LandscapeParameters& params);
    void applySmooth(Image& heightmap, int centerX, int centerY, float radius, float strength, const LandscapeParameters& params);
    void applyFlatten(Image& heightmap, int centerX, int centerY, float radius, float strength, const LandscapeParameters& params);
};
