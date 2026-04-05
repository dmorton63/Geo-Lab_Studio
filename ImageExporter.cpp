#include "ImageExporter.h"
#include "LandscapeDesign.h"

#pragma warning(push)
#pragma warning(disable: 4996)
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#pragma warning(pop)

#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <windows.h>

bool ImageExporter::exportImage(const Image& image, const std::string& filename, 
                                ExportFormat format, ExportMode mode) {
    if (image.size() == 0) {
        std::cerr << "Cannot export empty image" << std::endl;
        return false;
    }

    switch (format) {
        case ExportFormat::PNG:
            return exportPNG(image, filename, mode);
        case ExportFormat::RAW:
            return exportRAW(image, filename);
        default:
            return false;
    }
}

bool ImageExporter::exportCurrentView(const Image& image, const std::string& baseName, 
                                      int previewMode, ExportFormat format) {
    std::string modeName = getPreviewModeName(previewMode);
    std::string extension = (format == ExportFormat::PNG) ? ".png" : ".raw";
    std::string filename = baseName + "_" + modeName + extension;
    
    ExportMode mode = (previewMode == 3) ? ExportMode::BiomeColored : ExportMode::Grayscale;
    
    return exportImage(image, filename, format, mode);
}

bool ImageExporter::exportPNG(const Image& image, const std::string& filename, ExportMode mode) {
    size_t width = image.width();
    size_t height = image.height();
    
    int channels = (mode == ExportMode::BiomeColored) ? 3 : 1;
    std::vector<unsigned char> buffer(width * height * channels);
    
    if (mode == ExportMode::Grayscale) {
        imageToGrayscale(image, buffer.data());
    } else {
        imageToBiomeColor(image, buffer.data());
    }
    
    int result = stbi_write_png(filename.c_str(), 
                                static_cast<int>(width), 
                                static_cast<int>(height), 
                                channels, 
                                buffer.data(), 
                                static_cast<int>(width * channels));
    
    if (result) {
        std::cout << "Successfully exported PNG: " << filename << std::endl;
        return true;
    } else {
        std::cerr << "Failed to export PNG: " << filename << std::endl;
        return false;
    }
}

bool ImageExporter::exportRAW(const Image& image, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }
    
    // Write header: width, height (4 bytes each)
    uint32_t width = static_cast<uint32_t>(image.width());
    uint32_t height = static_cast<uint32_t>(image.height());
    file.write(reinterpret_cast<const char*>(&width), sizeof(uint32_t));
    file.write(reinterpret_cast<const char*>(&height), sizeof(uint32_t));
    
    // Write float data
    const std::vector<float>& data = image.data();
    file.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(float));
    
    file.close();
    
    if (file.good()) {
        std::cout << "Successfully exported RAW: " << filename << std::endl;
        return true;
    } else {
        std::cerr << "Failed to export RAW: " << filename << std::endl;
        return false;
    }
}

std::string ImageExporter::getPreviewModeName(int previewMode) {
    switch (previewMode) {
        case 0: return "height";
        case 1: return "slope";
        case 2: return "curvature";
        case 3: return "biome";
        case 4: return "water";
        default: return "unknown";
    }
}

void ImageExporter::imageToGrayscale(const Image& image, unsigned char* buffer) {
    for (size_t i = 0; i < image.size(); ++i) {
        float value = std::clamp(image.at(i), 0.0f, 1.0f);
        buffer[i] = static_cast<unsigned char>(value * 255.0f);
    }
}

void ImageExporter::imageToBiomeColor(const Image& image, unsigned char* buffer) {
    for (size_t i = 0; i < image.size(); ++i) {
        int biomeType = static_cast<int>(image.at(i));
        size_t idx = i * 3;
        
        switch (biomeType) {
            case static_cast<int>(BiomeType::Water):
                buffer[idx + 0] = 50;   // R
                buffer[idx + 1] = 100;  // G
                buffer[idx + 2] = 200;  // B (Blue water)
                break;
            case static_cast<int>(BiomeType::Grass):
                buffer[idx + 0] = 50;   // R
                buffer[idx + 1] = 180;  // G (Green grass)
                buffer[idx + 2] = 50;   // B
                break;
            case static_cast<int>(BiomeType::Rock):
                buffer[idx + 0] = 150;  // R
                buffer[idx + 1] = 150;  // G
                buffer[idx + 2] = 150;  // B (Gray rock)
                break;
            default:
                buffer[idx + 0] = 0;
                buffer[idx + 1] = 0;
                buffer[idx + 2] = 0;
                break;
        }
    }
}

// **NEW: Heightmap export with file dialog**
HeightmapExportResult ImageExporter::exportHeightmapWithDialog(const Image& heightmap, ExportFormat defaultFormat) {
    HeightmapExportResult result;

    if (heightmap.size() == 0) {
        result.errorMessage = "Cannot export empty heightmap";
        return result;
    }

    std::string filePath = showSaveFileDialog(defaultFormat);
    if (filePath.empty()) {
        result.errorMessage = "No file selected";
        return result;
    }

    // Detect format from extension
    ExportFormat format = detectFormatFromPath(filePath);

    bool success = false;
    switch (format) {
        case ExportFormat::RAW_8BIT:
            success = exportRAW8(heightmap, filePath);
            break;
        case ExportFormat::RAW_16BIT:
            success = exportRAW16(heightmap, filePath);
            break;
        case ExportFormat::RAW_32BIT:
            success = exportRAW32Float(heightmap, filePath);
            break;
        case ExportFormat::PNG:
            success = exportPNG(heightmap, filePath, ExportMode::Grayscale);
            break;
        default:
            result.errorMessage = "Unsupported export format";
            return result;
    }

    if (success) {
        result.success = true;
        result.filePath = filePath;
    } else {
        result.errorMessage = "Failed to write file";
    }

    return result;
}

// **NEW: Windows file save dialog**
std::string ImageExporter::showSaveFileDialog(ExportFormat defaultFormat) {
    OPENFILENAMEA ofn;
    char fileName[MAX_PATH] = "heightmap";

    // Set default extension based on format
    switch (defaultFormat) {
        case ExportFormat::RAW_8BIT:
            strcat_s(fileName, ".r8");
            break;
        case ExportFormat::RAW_16BIT:
            strcat_s(fileName, ".raw");
            break;
        case ExportFormat::RAW_32BIT:
            strcat_s(fileName, ".r32");
            break;
        case ExportFormat::PNG:
            strcat_s(fileName, ".png");
            break;
        default:
            strcat_s(fileName, ".raw");
            break;
    }

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "16-bit RAW (*.raw, *.r16)\0*.raw;*.r16\0"
                      "32-bit Float RAW (*.r32)\0*.r32\0"
                      "8-bit RAW (*.r8)\0*.r8\0"
                      "8-bit PNG (*.png)\0*.png\0"
                      "All Files\0*.*\0";
    ofn.nFilterIndex = 1; // Default to 16-bit RAW
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

    if (GetSaveFileNameA(&ofn) == TRUE) {
        return std::string(fileName);
    }

    return "";
}

// **NEW: Detect format from file extension**
ExportFormat ImageExporter::detectFormatFromPath(const std::string& filePath) {
    std::string ext = filePath.substr(filePath.find_last_of(".") + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "r16" || ext == "raw") {
        return ExportFormat::RAW_16BIT;
    } else if (ext == "r32") {
        return ExportFormat::RAW_32BIT;
    } else if (ext == "r8") {
        return ExportFormat::RAW_8BIT;
    } else if (ext == "png") {
        return ExportFormat::PNG;
    }

    return ExportFormat::RAW_16BIT; // Default
}

// **NEW: Export 8-bit RAW**
bool ImageExporter::exportRAW8(const Image& image, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to create file: " << filename << std::endl;
        return false;
    }

    // Convert [0.0, 1.0] to [0, 255]
    std::vector<uint8_t> buffer(image.size());
    for (size_t i = 0; i < image.size(); ++i) {
        float value = std::clamp(image.at(i), 0.0f, 1.0f);
        buffer[i] = static_cast<uint8_t>(value * 255.0f);
    }

    file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    file.close();

    if (file.good()) {
        std::cout << "Successfully exported 8-bit RAW: " << filename << std::endl;
        return true;
    }

    std::cerr << "Failed to export 8-bit RAW: " << filename << std::endl;
    return false;
}

// **NEW: Export 16-bit RAW**
bool ImageExporter::exportRAW16(const Image& image, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to create file: " << filename << std::endl;
        return false;
    }

    // Convert [0.0, 1.0] to [0, 65535]
    std::vector<uint16_t> buffer(image.size());
    for (size_t i = 0; i < image.size(); ++i) {
        float value = std::clamp(image.at(i), 0.0f, 1.0f);
        buffer[i] = static_cast<uint16_t>(value * 65535.0f);
    }

    file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size() * sizeof(uint16_t));
    file.close();

    if (file.good()) {
        std::cout << "Successfully exported 16-bit RAW: " << filename << std::endl;
        return true;
    }

    std::cerr << "Failed to export 16-bit RAW: " << filename << std::endl;
    return false;
}

// **NEW: Export 32-bit float RAW**
bool ImageExporter::exportRAW32Float(const Image& image, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to create file: " << filename << std::endl;
        return false;
    }

    // Write raw float data (already in [0.0, 1.0] range)
    const std::vector<float>& data = image.data();
    file.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(float));
    file.close();

    if (file.good()) {
        std::cout << "Successfully exported 32-bit float RAW: " << filename << std::endl;
        return true;
    }

    std::cerr << "Failed to export 32-bit float RAW: " << filename << std::endl;
    return false;
}
