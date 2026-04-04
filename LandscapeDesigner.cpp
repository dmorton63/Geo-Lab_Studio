#include "LandscapeDesigner.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <random>
#include <execution>
#include <numeric>
#include <vector>

Image LandscapeDesigner::normalize(const Image& src) {
    if (src.size() == 0) {
        return Image();
    }

    float minVal = std::numeric_limits<float>::max();
    float maxVal = std::numeric_limits<float>::lowest();

    for (size_t i = 0; i < src.size(); ++i) {
        minVal = std::min(minVal, src.at(i));
        maxVal = std::max(maxVal, src.at(i));
    }

    Image result(src.width(), src.height());
    
    float range = maxVal - minVal;
    if (range < 1e-6f) {
        return result;
    }

    for (size_t i = 0; i < src.size(); ++i) {
        result.at(i) = (src.at(i) - minVal) / range;
    }

    return result;
}

Image LandscapeDesigner::computeSlope(const Image& height) {
    if (height.width() < 2 || height.height() < 2) {
        return Image(height.width(), height.height(), 0.0f);
    }

    Image slope(height.width(), height.height(), 0.0f);

    // Parallelize slope computation
    std::vector<size_t> indices((height.width() - 2) * (height.height() - 2));
    std::iota(indices.begin(), indices.end(), 0);

    std::for_each(std::execution::par, indices.begin(), indices.end(),
        [&](size_t idx) {
            size_t x = (idx % (height.width() - 2)) + 1;
            size_t y = (idx / (height.width() - 2)) + 1;

            float dx = (height(x + 1, y) - height(x - 1, y)) * 0.5f;
            float dy = (height(x, y + 1) - height(x, y - 1)) * 0.5f;

            slope(x, y) = std::sqrt(dx * dx + dy * dy);
        });

    return slope;
}

Image LandscapeDesigner::computeCurvature(const Image& height) {
    if (height.width() < 3 || height.height() < 3) {
        return Image(height.width(), height.height(), 0.0f);
    }

    Image curvature(height.width(), height.height(), 0.0f);

    // Parallelize curvature computation
    std::vector<size_t> indices((height.width() - 2) * (height.height() - 2));
    std::iota(indices.begin(), indices.end(), 0);

    std::for_each(std::execution::par, indices.begin(), indices.end(),
        [&](size_t idx) {
            size_t x = (idx % (height.width() - 2)) + 1;
            size_t y = (idx / (height.width() - 2)) + 1;

            float center = height(x, y);
            float laplacian = 
                height(x - 1, y) + 
                height(x + 1, y) + 
                height(x, y - 1) + 
                height(x, y + 1) - 
                4.0f * center;

            curvature(x, y) = laplacian;
        });

    return curvature;
}

Image LandscapeDesigner::computeWaterMask(const Image& height, float waterLevel) {
    Image waterMask(height.width(), height.height(), 0.0f);

    for (size_t i = 0; i < height.size(); ++i) {
        waterMask.at(i) = (height.at(i) < waterLevel) ? 1.0f : 0.0f;
    }

    return waterMask;
}

Image LandscapeDesigner::computeBiomeMask(const Image& slope, const Image& waterMask, float slopeThreshold) {
    Image biomeMask(slope.width(), slope.height(), 0.0f);

    for (size_t i = 0; i < slope.size(); ++i) {
        if (waterMask.at(i) > 0.5f) {
            biomeMask.at(i) = static_cast<float>(BiomeType::Water);
        } else if (slope.at(i) > slopeThreshold) {
            biomeMask.at(i) = static_cast<float>(BiomeType::Rock);
        } else {
            biomeMask.at(i) = static_cast<float>(BiomeType::Grass);
        }
    }

    return biomeMask;
}

Image LandscapeDesigner::addNoise(const Image& height, float noiseAmount) {
    if (noiseAmount < 0.001f) {
        return height;
    }

    Image result = height;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-noiseAmount, noiseAmount);

    for (size_t i = 0; i < result.size(); ++i) {
        result.at(i) += dis(gen);
        result.at(i) = std::clamp(result.at(i), 0.0f, 1.0f);
    }

    return result;
}

LandscapeDesign LandscapeDesigner::designLandscape(const Image& rawHeight, float waterLevel) {
    LandscapeDesign out;

    out.height = normalize(rawHeight);
    out.slope = computeSlope(out.height);
    out.curvature = computeCurvature(out.height);
    out.waterMask = computeWaterMask(out.height, waterLevel);
    out.biomeMask = computeBiomeMask(out.slope, out.waterMask, 0.01f);

    return out;
}

LandscapeDesign LandscapeDesigner::designLandscape(const Image& rawHeight, const LandscapeParameters& params) {
    LandscapeDesign out;

    out.height = normalize(rawHeight);

    if (params.noiseAmount > 0.001f) {
        out.height = addNoise(out.height, params.noiseAmount);
    }

    out.slope = computeSlope(out.height);
    out.curvature = computeCurvature(out.height);
    out.waterMask = computeWaterMask(out.height, params.waterLevel);
    out.biomeMask = computeBiomeMask(out.slope, out.waterMask, params.slopeThreshold);

    return out;
}
