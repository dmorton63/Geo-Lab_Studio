#pragma once
#include "Image.h"
#include "LandscapeParameters.h"
#include <vector>

class BrushTool {
public:
    BrushTool();

    void applyBrush(Image& heightmap, int centerX, int centerY, BrushType type, 
                    float radius, float strength);

    void setFlattenHeight(float height) { flattenHeight = height; }
    float getFlattenHeight() const { return flattenHeight; }

private:
    float flattenHeight;

    float calculateBrushWeight(float distance, float radius) const;
    void applyRaise(Image& heightmap, int centerX, int centerY, float radius, float strength);
    void applyLower(Image& heightmap, int centerX, int centerY, float radius, float strength);
    void applySmooth(Image& heightmap, int centerX, int centerY, float radius, float strength);
    void applyFlatten(Image& heightmap, int centerX, int centerY, float radius, float strength);
};
