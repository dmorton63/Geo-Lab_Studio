#pragma once

#define NOMINMAX  // Prevent Windows.h from defining min/max macros
#include "Image.h"
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <windows.h> // For file dialog

// Supported heightmap import formats
enum class ImportFormat {
    RAW_8BIT,
    RAW_16BIT,
    RAW_32BIT_FLOAT,
    PNG_8BIT,
    PNG_16BIT,
    UNKNOWN
};

// Import result structure
struct ImportResult {
    bool success = false;
    std::string errorMessage;
    Image heightmap;
    size_t width = 0;
    size_t height = 0;
    float minValue = 0.0f;
    float maxValue = 1.0f;
};

class ImageImporter {
public:
    // Show Windows file open dialog and import selected heightmap
    static ImportResult importWithDialog(size_t targetResolution = 0) {
        std::string filePath = showOpenFileDialog();
        if (filePath.empty()) {
            ImportResult result;
            result.errorMessage = "No file selected";
            return result;
        }
        
        return importHeightmap(filePath, targetResolution);
    }
    
    // Import heightmap from file path
    static ImportResult importHeightmap(const std::string& filePath, size_t targetResolution = 0) {
        ImportFormat format = detectFormat(filePath);
        
        switch (format) {
            case ImportFormat::RAW_16BIT:
                return importRAW16(filePath, targetResolution);
            case ImportFormat::RAW_32BIT_FLOAT:
                return importRAW32Float(filePath, targetResolution);
            case ImportFormat::RAW_8BIT:
                return importRAW8(filePath, targetResolution);
            default:
                ImportResult result;
                result.errorMessage = "Unsupported format. Use 16-bit RAW (.raw, .r16) or 32-bit float RAW (.r32)";
                return result;
        }
    }
    
private:
    // Show Windows file open dialog
    static std::string showOpenFileDialog() {
        OPENFILENAMEA ofn;
        char fileName[MAX_PATH] = "";
        
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = fileName;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrFilter = "Heightmap Files\0*.raw;*.r16;*.r32\0"
                          "16-bit RAW (*.raw, *.r16)\0*.raw;*.r16\0"
                          "32-bit Float RAW (*.r32)\0*.r32\0"
                          "All Files\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
        
        if (GetOpenFileNameA(&ofn) == TRUE) {
            return std::string(fileName);
        }
        
        return "";
    }
    
    // Detect format from file extension
    static ImportFormat detectFormat(const std::string& filePath) {
        std::string ext = filePath.substr(filePath.find_last_of(".") + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == "r16" || ext == "raw") {
            return ImportFormat::RAW_16BIT;
        } else if (ext == "r32") {
            return ImportFormat::RAW_32BIT_FLOAT;
        } else if (ext == "r8") {
            return ImportFormat::RAW_8BIT;
        }
        
        return ImportFormat::UNKNOWN;
    }
    
    // Import 8-bit RAW heightmap
    static ImportResult importRAW8(const std::string& filePath, size_t targetResolution) {
        ImportResult result;
        
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            result.errorMessage = "Failed to open file";
            return result;
        }
        
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        // Determine resolution from file size
        size_t resolution = static_cast<size_t>(std::sqrt(fileSize));
        if (resolution * resolution != fileSize) {
            result.errorMessage = "File size is not a perfect square. Cannot determine resolution.";
            return result;
        }
        
        // Read data
        std::vector<uint8_t> buffer(fileSize);
        file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
        file.close();
        
        if (!file) {
            result.errorMessage = "Error reading file";
            return result;
        }
        
        // Convert to Image (normalize to 0.0-1.0)
        Image img(resolution, resolution);
        for (size_t i = 0; i < fileSize; ++i) {
            img.at(i) = static_cast<float>(buffer[i]) / 255.0f;
        }
        
        result.success = true;
        result.width = resolution;
        result.height = resolution;
        result.heightmap = std::move(img);
        result.minValue = 0.0f;
        result.maxValue = 1.0f;
        
        return result;
    }
    
    // Import 16-bit RAW heightmap
    static ImportResult importRAW16(const std::string& filePath, size_t targetResolution) {
        ImportResult result;
        
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            result.errorMessage = "Failed to open file";
            return result;
        }
        
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        // Each pixel is 2 bytes (uint16_t)
        size_t pixelCount = fileSize / sizeof(uint16_t);
        size_t resolution = static_cast<size_t>(std::sqrt(pixelCount));
        
        if (resolution * resolution != pixelCount) {
            result.errorMessage = "File size is not a perfect square. Cannot determine resolution.";
            return result;
        }
        
        // Read data
        std::vector<uint16_t> buffer(pixelCount);
        file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
        file.close();
        
        if (!file) {
            result.errorMessage = "Error reading file";
            return result;
        }
        
        // Find min/max for normalization
        auto [minIt, maxIt] = std::minmax_element(buffer.begin(), buffer.end());
        float minVal = static_cast<float>(*minIt);
        float maxVal = static_cast<float>(*maxIt);
        
        // Convert to Image (normalize to 0.0-1.0)
        Image img(resolution, resolution);
        for (size_t i = 0; i < pixelCount; ++i) {
            float normalized = (static_cast<float>(buffer[i]) - minVal) / (maxVal - minVal + 1e-6f);
            img.at(i) = normalized;
        }
        
        result.success = true;
        result.width = resolution;
        result.height = resolution;
        result.heightmap = std::move(img);
        result.minValue = minVal / 65535.0f;
        result.maxValue = maxVal / 65535.0f;
        
        return result;
    }
    
    // Import 32-bit float RAW heightmap
    static ImportResult importRAW32Float(const std::string& filePath, size_t targetResolution) {
        ImportResult result;
        
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            result.errorMessage = "Failed to open file";
            return result;
        }
        
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        // Each pixel is 4 bytes (float)
        size_t pixelCount = fileSize / sizeof(float);
        size_t resolution = static_cast<size_t>(std::sqrt(pixelCount));
        
        if (resolution * resolution != pixelCount) {
            result.errorMessage = "File size is not a perfect square. Cannot determine resolution.";
            return result;
        }
        
        // Read directly into Image
        Image img(resolution, resolution);
        file.read(reinterpret_cast<char*>(img.data().data()), fileSize);
        file.close();
        
        if (!file) {
            result.errorMessage = "Error reading file";
            return result;
        }
        
        // Find min/max
        auto [minIt, maxIt] = std::minmax_element(img.data().begin(), img.data().end());
        float minVal = *minIt;
        float maxVal = *maxIt;
        
        // Normalize to 0.0-1.0
        for (float& val : img.data()) {
            val = (val - minVal) / (maxVal - minVal + 1e-6f);
        }
        
        result.success = true;
        result.width = resolution;
        result.height = resolution;
        result.heightmap = std::move(img);
        result.minValue = minVal;
        result.maxValue = maxVal;
        
        return result;
    }
};
