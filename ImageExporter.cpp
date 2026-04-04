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
