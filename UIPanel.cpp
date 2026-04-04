#include "UIPanel.h"
#include "ImageExporter.h"
#include <imgui.h>
#include <random>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>

void UIPanel::render(LandscapeParameters& params, const LandscapeDesign* design, Camera* camera) {
    ImGui::Begin("Landscape Designer", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Landscape Designer v0.5");
    ImGui::Separator();

    renderPreviewSelector(params);

    ImGui::Spacing();

    if (camera) {
        render3DControls(params, camera);
        ImGui::Spacing();
    }

    renderPaintModeControls(params);

    ImGui::Spacing();

    renderErosionControls(params);

    ImGui::Spacing();

    renderParameterSliders(params);

    ImGui::Spacing();

    if (design) {
        renderStatistics(design);
    }

    ImGui::Spacing();

    if (design) {
        renderExportSection(params, design);
    }

    ImGui::End();
}

void UIPanel::renderParameterSliders(LandscapeParameters& params) {
    if (ImGui::CollapsingHeader("Terrain Generation", ImGuiTreeNodeFlags_DefaultOpen)) {
        const char* noiseTypes[] = { "FBM (Natural)", "Ridged (Mountains)", "Billowy (Soft)", "Classic (Sin/Cos)" };
        int currentNoiseType = static_cast<int>(params.noiseType);
        if (ImGui::Combo("Noise Type", &currentNoiseType, noiseTypes, 4)) {
            params.noiseType = static_cast<NoiseType>(currentNoiseType);
            params.markChanged();
        }

        if (params.noiseType != NoiseType::Classic) {
            if (ImGui::SliderFloat("Scale", &params.noiseScale, 1.0f, 10.0f)) {
                params.markChanged();
            }
            if (ImGui::SliderInt("Octaves", &params.noiseOctaves, 1, 8)) {
                params.markChanged();
            }
            if (ImGui::SliderFloat("Persistence", &params.noisePersistence, 0.1f, 1.0f)) {
                params.markChanged();
            }
            if (ImGui::SliderFloat("Lacunarity", &params.noiseLacunarity, 1.5f, 3.5f)) {
                params.markChanged();
            }
        } else {
            if (ImGui::SliderFloat("Heightmap Frequency", &params.heightmapFrequency, 1.0f, 50.0f)) {
                params.markChanged();
            }
        }

        ImGui::Separator();

        if (ImGui::InputInt("Seed", reinterpret_cast<int*>(&params.seed))) {
            params.markChanged();
        }

        if (ImGui::Button("Randomize Seed", ImVec2(-1, 0))) {
            std::random_device rd;
            params.seed = rd();
            params.markChanged();
        }

        const char* resolutions[] = { "128", "256", "512", "1024" };
        int currentRes = 1;
        if (params.mapResolution == 128) currentRes = 0;
        else if (params.mapResolution == 256) currentRes = 1;
        else if (params.mapResolution == 512) currentRes = 2;
        else if (params.mapResolution == 1024) currentRes = 3;

        if (ImGui::Combo("Resolution", &currentRes, resolutions, 4)) {
            params.mapResolution = (currentRes == 0) ? 128 : (currentRes == 1) ? 256 : (currentRes == 2) ? 512 : 1024;
            params.markChanged();
        }
    }

    if (ImGui::CollapsingHeader("Biome Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::SliderFloat("Water Level", &params.waterLevel, 0.0f, 1.0f)) {
            params.markChanged();
        }

        if (ImGui::SliderFloat("Slope Threshold", &params.slopeThreshold, 0.001f, 0.5f)) {
            params.markChanged();
        }
    }

    if (ImGui::CollapsingHeader("Advanced")) {
        if (ImGui::SliderFloat("Noise Amount", &params.noiseAmount, 0.0f, 1.0f)) {
            params.markChanged();
        }

        ImGui::BeginDisabled(true);
        ImGui::SliderFloat("Erosion Strength", &params.erosionStrength, 0.0f, 1.0f);
        ImGui::Text("(Coming Soon)");
        ImGui::EndDisabled();
    }

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::Checkbox("Auto Update", &params.autoUpdate);

    if (!params.autoUpdate && ImGui::Button("Regenerate", ImVec2(-1, 0))) {
        params.markChanged();
    }
}

void UIPanel::renderPreviewSelector(LandscapeParameters& params) {
    const char* previews[] = { "Height", "Slope", "Curvature", "Biome", "Water" };

    ImGui::Text("Preview Mode");
    if (ImGui::Combo("##PreviewMode", &params.previewMode, previews, 5)) {
        // Preview mode changed, no need to regenerate, just switch display
    }

    ImGui::Separator();
}

void UIPanel::renderStatistics(const LandscapeDesign* design) {
    if (ImGui::CollapsingHeader("Statistics", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (!design || design->biomeMask.size() == 0) {
            ImGui::Text("No data");
            return;
        }

        int waterPixels = 0;
        int grassPixels = 0;
        int rockPixels = 0;

        for (size_t i = 0; i < design->biomeMask.size(); ++i) {
            int biome = static_cast<int>(design->biomeMask.at(i));
            if (biome == static_cast<int>(BiomeType::Water)) waterPixels++;
            else if (biome == static_cast<int>(BiomeType::Grass)) grassPixels++;
            else if (biome == static_cast<int>(BiomeType::Rock)) rockPixels++;
        }

        int total = waterPixels + grassPixels + rockPixels;

        ImGui::Text("Water: %d (%.1f%%)", waterPixels, (waterPixels * 100.0f) / total);
        ImGui::Text("Grass: %d (%.1f%%)", grassPixels, (grassPixels * 100.0f) / total);
        ImGui::Text("Rock:  %d (%.1f%%)", rockPixels, (rockPixels * 100.0f) / total);

        ImGui::Spacing();
        ImGui::Text("Resolution: %dx%d", (int)design->height.width(), (int)design->height.height());
    }
}

void UIPanel::renderExportSection(LandscapeParameters& params, const LandscapeDesign* design) {
    if (ImGui::CollapsingHeader("Export")) {
        if (!design || design->height.size() == 0) {
            ImGui::Text("Generate terrain first");
            return;
        }

        ImGui::Text("Export current view:");

        // Generate timestamped filename
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
        localtime_s(&tm, &time);

        std::stringstream ss;
        ss << "landscape_" << params.seed << "_" 
           << std::put_time(&tm, "%Y%m%d_%H%M%S");
        std::string baseName = ss.str();

        ImGui::Text("Filename: %s", baseName.c_str());

        ImGui::Spacing();

        // Export buttons
        if (ImGui::Button("Export as PNG", ImVec2(-1, 0))) {
            const Image* exportImage = nullptr;

            switch (params.previewMode) {
                case 0: exportImage = &design->height; break;
                case 1: exportImage = &design->slope; break;
                case 2: exportImage = &design->curvature; break;
                case 3: exportImage = &design->biomeMask; break;
                case 4: exportImage = &design->waterMask; break;
            }

            if (exportImage) {
                ImageExporter::exportCurrentView(*exportImage, baseName, 
                                                 params.previewMode, ExportFormat::PNG);
            }
        }

        if (ImGui::Button("Export as RAW", ImVec2(-1, 0))) {
            const Image* exportImage = nullptr;

            switch (params.previewMode) {
                case 0: exportImage = &design->height; break;
                case 1: exportImage = &design->slope; break;
                case 2: exportImage = &design->curvature; break;
                case 3: exportImage = &design->biomeMask; break;
                case 4: exportImage = &design->waterMask; break;
            }

            if (exportImage) {
                ImageExporter::exportCurrentView(*exportImage, baseName, 
                                                 params.previewMode, ExportFormat::RAW);
            }
        }

        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::Button("Export All Layers (PNG)", ImVec2(-1, 0))) {
            ImageExporter::exportCurrentView(design->height, baseName, 0, ExportFormat::PNG);
            ImageExporter::exportCurrentView(design->slope, baseName, 1, ExportFormat::PNG);
            ImageExporter::exportCurrentView(design->curvature, baseName, 2, ExportFormat::PNG);
            ImageExporter::exportCurrentView(design->biomeMask, baseName, 3, ExportFormat::PNG);
            ImageExporter::exportCurrentView(design->waterMask, baseName, 4, ExportFormat::PNG);
        }

        ImGui::Text("Exports saved to executable directory");
    }
}

void UIPanel::render3DControls(LandscapeParameters& params, Camera* camera) {
    if (ImGui::CollapsingHeader("3D View", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Checkbox("Enable 3D View", &params.view3D)) {
            params.markChanged();
        }

        if (params.view3D) {
            ImGui::Spacing();

            if (ImGui::SliderFloat("Height Scale", &params.heightScale, 0.1f, 2.0f)) {
                params.markChanged();
            }

            ImGui::Spacing();
            ImGui::Text("Camera Controls:");
            ImGui::BulletText("Left Click + Drag: Rotate");
            ImGui::BulletText("Right Click + Drag: Pan");
            ImGui::BulletText("Mouse Wheel: Zoom");

            ImGui::Spacing();

            if (ImGui::Button("Reset Camera", ImVec2(-1, 0))) {
                if (camera) {
                    camera->reset();
                }
            }

            ImGui::Spacing();
            if (camera) {
                ImGui::Text("Distance: %.2f", camera->getDistance());
                ImGui::Text("Yaw: %.1f", camera->getYaw());
                ImGui::Text("Pitch: %.1f", camera->getPitch());
            }
        }
    }
}

void UIPanel::renderPaintModeControls(LandscapeParameters& params) {
    if (ImGui::CollapsingHeader("Paint Mode", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::Checkbox("Enable Paint Mode", &params.paintMode)) {
            if (params.paintMode) {
                params.view3D = false;
                params.autoUpdate = false;
            }
        }

        if (params.paintMode) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Paint Mode Active - Auto-update disabled");
            ImGui::Spacing();

            const char* brushTypes[] = { "Raise", "Lower", "Smooth", "Flatten" };
            int currentBrushType = static_cast<int>(params.brushType);
            if (ImGui::Combo("Brush Type", &currentBrushType, brushTypes, 4)) {
                params.brushType = static_cast<BrushType>(currentBrushType);
            }

            ImGui::SliderFloat("Brush Radius", &params.brushRadius, 5.0f, 100.0f);
            ImGui::SliderFloat("Brush Strength", &params.brushStrength, 1.0f, 100.0f);

            if (params.brushType == BrushType::Flatten) {
                ImGui::Spacing();
                ImGui::SliderFloat("Flatten Height", &params.flattenHeight, 0.0f, 1.0f);
                ImGui::SameLine();
                if (ImGui::Button("Sample")) {
                    // Will be set from main.cpp when clicking
                }
            }

            ImGui::Spacing();
            ImGui::Text("Controls:");
            ImGui::BulletText("Left Click + Drag: Paint");
            ImGui::BulletText("Ctrl+Z: Undo (coming soon)");

            ImGui::Spacing();
            if (ImGui::Button("Exit Paint Mode", ImVec2(-1, 0))) {
                params.paintMode = false;
                params.autoUpdate = true;
            }
        }
    }
}

void UIPanel::renderErosionControls(LandscapeParameters& params) {
    if (ImGui::CollapsingHeader("Erosion Simulation", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::TextWrapped("Apply realistic weathering effects to terrain. Works on generated or painted terrain.");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Hydraulic Erosion
        if (ImGui::TreeNode("Hydraulic Erosion (Water)")) {
            ImGui::TextWrapped("Simulates water droplets carving valleys and depositing sediment.");
            ImGui::Spacing();

            ImGui::SliderInt("Droplets", &params.hydraulicIterations, 0, 200000, "%d", ImGuiSliderFlags_Logarithmic);
            ImGui::SliderFloat("Rain Amount", &params.rainAmount, 0.1f, 5.0f);
            ImGui::SliderFloat("Evaporation", &params.evaporation, 0.001f, 0.1f, "%.3f");
            ImGui::SliderFloat("Erosion Strength", &params.hydraulicStrength, 0.1f, 1.0f);

            ImGui::TreePop();
        }

        ImGui::Spacing();

        // Thermal Erosion
        if (ImGui::TreeNode("Thermal Erosion (Gravity)")) {
            ImGui::TextWrapped("Simulates material sliding down steep slopes.");
            ImGui::Spacing();

            ImGui::SliderInt("Iterations", &params.thermalIterations, 0, 20);
            ImGui::SliderFloat("Talus Angle", &params.talusAngle, 0.01f, 0.2f, "%.3f");
            ImGui::SliderFloat("Erosion Strength", &params.thermalStrength, 0.1f, 1.0f);

            ImGui::TreePop();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Apply buttons
        if (ImGui::Button("Apply Hydraulic Only", ImVec2(-1, 0))) {
            params.erosionMode = true;
            params.markChanged();
        }

        if (ImGui::Button("Apply Thermal Only", ImVec2(-1, 0))) {
            params.erosionMode = true;
            params.markChanged();
        }

        if (ImGui::Button("Apply Both (Recommended)", ImVec2(-1, 0))) {
            params.erosionMode = true;
            params.markChanged();
        }

        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), 
                          "Note: Erosion modifies current terrain.");
        ImGui::TextWrapped("Export before eroding if you want to keep the original.");
    }
}
