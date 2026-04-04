#include "DataDumper.h"
#include <fstream>
#include <iomanip>
#include <iostream>

void DataDumper::dumpHeightmap(const Image& heightmap, const std::string& filename) {
    if (heightmap.size() == 0) {
        std::cerr << "Cannot dump empty heightmap" << std::endl;
        return;
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    file << "Heightmap Dump - Size: " << heightmap.width() << "x" << heightmap.height() << "\n";
    file << "Format: [Y][X] = Value (range 0.0 to 1.0)\n";
    file << "Origin (0,0) is TOP-LEFT corner\n";
    file << "========================================\n\n";

    file << std::fixed << std::setprecision(3);

    // Dump full data in grid format
    for (size_t y = 0; y < heightmap.height(); ++y) {
        for (size_t x = 0; x < heightmap.width(); ++x) {
            file << std::setw(6) << heightmap(x, y) << " ";
        }
        file << "\n";
    }

    file.close();
    std::cout << "Heightmap dumped to: " << filename << std::endl;
}

void DataDumper::dumpSample(const Image& heightmap, const std::string& filename,
                           int startX, int startY, int width, int height) {
    if (heightmap.size() == 0) {
        std::cerr << "Cannot dump empty heightmap" << std::endl;
        return;
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    int endX = std::min(startX + width, static_cast<int>(heightmap.width()));
    int endY = std::min(startY + height, static_cast<int>(heightmap.height()));

    file << "Heightmap Sample Dump\n";
    file << "Full Size: " << heightmap.width() << "x" << heightmap.height() << "\n";
    file << "Sample Region: X[" << startX << "-" << endX << "] Y[" << startY << "-" << endY << "]\n";
    file << "Origin (0,0) is TOP-LEFT corner\n";
    file << "========================================\n\n";

    file << std::fixed << std::setprecision(3);

    file << "     ";
    for (int x = startX; x < endX; ++x) {
        file << "X" << std::setw(4) << x << " ";
    }
    file << "\n";

    for (int y = startY; y < endY; ++y) {
        file << "Y" << std::setw(3) << y << ": ";
        for (int x = startX; x < endX; ++x) {
            file << std::setw(6) << heightmap(x, y) << " ";
        }
        file << "\n";
    }

    file.close();
    std::cout << "Heightmap sample dumped to: " << filename << std::endl;
}
