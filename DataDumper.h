#pragma once
#include "Image.h"
#include <string>

class DataDumper {
public:
    // Dump heightmap data to a text file
    static void dumpHeightmap(const Image& heightmap, const std::string& filename);
    
    // Dump a small sample area for quick inspection
    static void dumpSample(const Image& heightmap, const std::string& filename, 
                          int startX, int startY, int width, int height);
};
