#pragma once

#define NOMINMAX  // Prevent Windows.h from defining min/max macros
#include "Image.h"
#include <string>
#include <windows.h> // For file dialog

enum class ExportFormat {
    PNG,
    RAW,
    RAW_8BIT,   // 8-bit unsigned RAW
    RAW_16BIT,  // 16-bit unsigned RAW
    RAW_32BIT   // 32-bit float RAW
};

enum class ExportMode {
    Grayscale,      // For height, slope, curvature, water mask
    BiomeColored    // For biome mask with color coding
};

// Export result structure
struct HeightmapExportResult {
    bool success = false;
    std::string errorMessage;
    std::string filePath;
};

class ImageExporter {
public:
    // Export a single image
    static bool exportImage(const Image& image, const std::string& filename, 
                           ExportFormat format, ExportMode mode = ExportMode::Grayscale);

    // Export with automatic naming based on preview mode
    static bool exportCurrentView(const Image& image, const std::string& baseName, 
                                  int previewMode, ExportFormat format);

    // **NEW: Heightmap export with file dialog**
    static HeightmapExportResult exportHeightmapWithDialog(const Image& heightmap, ExportFormat defaultFormat = ExportFormat::RAW_16BIT);

private:
    static bool exportPNG(const Image& image, const std::string& filename, ExportMode mode);
    static bool exportRAW(const Image& image, const std::string& filename);

    // **NEW: RAW export variants**
    static bool exportRAW8(const Image& image, const std::string& filename);
    static bool exportRAW16(const Image& image, const std::string& filename);
    static bool exportRAW32Float(const Image& image, const std::string& filename);

    static std::string getPreviewModeName(int previewMode);
    static void imageToGrayscale(const Image& image, unsigned char* buffer);
    static void imageToBiomeColor(const Image& image, unsigned char* buffer);

    // **NEW: Windows file dialog**
    static std::string showSaveFileDialog(ExportFormat defaultFormat);
    static ExportFormat detectFormatFromPath(const std::string& filePath);
};
