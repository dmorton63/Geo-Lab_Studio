#pragma once
#include "Image.h"

struct LandscapeDesign {
    Image height;
    Image slope;
    Image curvature;
    Image biomeMask;
    Image waterMask;
};

enum class BiomeType : int {
    Water = 0,
    Grass = 1,
    Rock = 2
};
