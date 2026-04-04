#pragma once
#include "Image.h"
#include "LandscapeDesign.h"
#include "LandscapeParameters.h"

class LandscapeDesigner {
public:
    static Image normalize(const Image& src);

    static Image computeSlope(const Image& height);

    static Image computeCurvature(const Image& height);

    static Image computeWaterMask(const Image& height, float waterLevel);

    static Image computeBiomeMask(const Image& slope, const Image& waterMask, float slopeThreshold);

    static Image addNoise(const Image& height, float noiseAmount);

    static LandscapeDesign designLandscape(const Image& rawHeight, float waterLevel);
    static LandscapeDesign designLandscape(const Image& rawHeight, const LandscapeParameters& params);
};
