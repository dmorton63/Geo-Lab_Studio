#pragma once
#include "Image.h"
#include <string>

enum class ExportFormat {
    PNG,
    RAW
};

enum class ExportMode {
    Grayscale,      // For height, slope, curvature, water mask
    BiomeColored    // For biome mask with color coding
};

class ImageExporter {
public:
    // Export a single image
    static bool exportImage(const Image& image, const std::string& filename, 
                           ExportFormat format, ExportMode mode = ExportMode::Grayscale);
    
    // Export with automatic naming based on preview mode
    static bool exportCurrentView(const Image& image, const std::string& baseName, 
                                  int previewMode, ExportFormat format);
    
private:
    static bool exportPNG(const Image& image, const std::string& filename, ExportMode mode);
    static bool exportRAW(const Image& image, const std::string& filename);
    
    static std::string getPreviewModeName(int previewMode);
    static void imageToGrayscale(const Image& image, unsigned char* buffer);
    static void imageToBiomeColor(const Image& image, unsigned char* buffer);
};
