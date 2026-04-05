#include "LandscapeDesigner.h"
#include "LandscapeParameters.h"
#include "PerlinNoise.h"
#include "Renderer.h"
#include "UIPanel.h"
#include "Camera.h"
#include "BrushTool.h"
#include "ErosionSimulator.h"
#include "DataDumper.h"
#include "Framebuffer.h"
#include "EngineScalingHelper.h"
#include "ImageImporter.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <cmath>
#include <random>
#include <execution>
#include <algorithm>
#include <vector>
#include <numeric>
#include <future>
#include <atomic>

// Resample an image to a new resolution using bilinear interpolation
Image resampleImage(const Image& source, size_t newWidth, size_t newHeight) {
    Image result(newWidth, newHeight);

    float xRatio = static_cast<float>(source.width() - 1) / static_cast<float>(newWidth - 1);
    float yRatio = static_cast<float>(source.height() - 1) / static_cast<float>(newHeight - 1);

    for (size_t y = 0; y < newHeight; ++y) {
        for (size_t x = 0; x < newWidth; ++x) {
            float srcX = x * xRatio;
            float srcY = y * yRatio;

            size_t x0 = static_cast<size_t>(srcX);
            size_t y0 = static_cast<size_t>(srcY);
            size_t x1 = std::min(x0 + 1, source.width() - 1);
            size_t y1 = std::min(y0 + 1, source.height() - 1);

            float fracX = srcX - x0;
            float fracY = srcY - y0;

            // Bilinear interpolation
            float v00 = source(x0, y0);
            float v10 = source(x1, y0);
            float v01 = source(x0, y1);
            float v11 = source(x1, y1);

            float v0 = v00 * (1.0f - fracX) + v10 * fracX;
            float v1 = v01 * (1.0f - fracX) + v11 * fracX;
            float value = v0 * (1.0f - fracY) + v1 * fracY;

            result(x, y) = value;
        }
    }

    return result;
}

Image generateHeightmap(const LandscapeParameters& params) {
    Image heightmap(params.mapResolution, params.mapResolution);
    
    if (params.noiseType == NoiseType::Classic) {
        std::mt19937 gen(params.seed);
        std::uniform_real_distribution<float> offsetDist(-100.0f, 100.0f);
        float xOffset = offsetDist(gen);
        float yOffset = offsetDist(gen);
        
        for (size_t y = 0; y < heightmap.height(); ++y) {
            for (size_t x = 0; x < heightmap.width(); ++x) {
                float nx = static_cast<float>(x) / heightmap.width() - 0.5f;
                float ny = static_cast<float>(y) / heightmap.height() - 0.5f;
                
                float value = 0.5f + 0.3f * std::sin((nx + xOffset * 0.01f) * params.heightmapFrequency) * 
                              std::cos((ny + yOffset * 0.01f) * params.heightmapFrequency);
                value += 0.2f * (1.0f - std::sqrt(nx * nx + ny * ny) * 2.0f);
                
                heightmap(x, y) = value;
            }
        }
    } else {
        PerlinNoise perlin(params.seed);
        
        // Parallelize Perlin noise generation
        std::vector<size_t> indices(heightmap.width() * heightmap.height());
        std::iota(indices.begin(), indices.end(), 0);
        
        std::for_each(std::execution::par, indices.begin(), indices.end(), 
            [&](size_t idx) {
                size_t x = idx % heightmap.width();
                size_t y = idx / heightmap.width();
                
                float nx = static_cast<float>(x) / heightmap.width();
                float ny = static_cast<float>(y) / heightmap.height();
                
                float noiseValue = 0.0f;
                switch (params.noiseType) {
                    case NoiseType::FBM:
                        noiseValue = perlin.fbm(nx * params.noiseScale, ny * params.noiseScale, 
                                                params.noiseOctaves, params.noisePersistence, params.noiseLacunarity);
                        break;
                    case NoiseType::Ridged:
                        noiseValue = perlin.ridgedNoise(nx * params.noiseScale, ny * params.noiseScale,
                                                        params.noiseOctaves, params.noisePersistence, params.noiseLacunarity);
                        break;
                    case NoiseType::Billowy:
                        noiseValue = perlin.billowyNoise(nx * params.noiseScale, ny * params.noiseScale,
                                                         params.noiseOctaves, params.noisePersistence, params.noiseLacunarity);
                        break;
                    default:
                        noiseValue = perlin.noise(nx * params.noiseScale, ny * params.noiseScale);
                }
                
                noiseValue = (noiseValue + 1.0f) * 0.5f;
                
                float radialGradient = 1.0f - std::sqrt((nx - 0.5f) * (nx - 0.5f) + (ny - 0.5f) * (ny - 0.5f)) * 1.4142f;
                radialGradient = std::max(0.0f, radialGradient);
                
                float value = noiseValue * 0.8f + radialGradient * 0.2f;
                heightmap(x, y) = value;
            });
    }

    return heightmap;
}

Image generateFlatTerrain(size_t resolution, float height = 0.5f) {
    Image heightmap(resolution, resolution);

    for (size_t y = 0; y < heightmap.height(); ++y) {
        for (size_t x = 0; x < heightmap.width(); ++x) {
            heightmap(x, y) = height;
        }
    }

    return heightmap;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(2400, 1200, "Landscape Designer V0.7.5 - Professional 3-Panel Layout", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    Renderer renderer;
    renderer.init();

    // Create framebuffers for dual viewport system
    Framebuffer fbo2D;  // For 2D paint view
    Framebuffer fbo3D;  // For 3D preview
    fbo2D.init(960, 1080);  // Left half of screen
    fbo3D.init(960, 1080);  // Right half of screen

    Camera camera;
    BrushTool brushTool;
    ErosionSimulator erosionSim;

    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    bool firstMouse = true;
    bool isPainting = false;

    LandscapeParameters params;
    LandscapeDesign design;
    Image rawHeight;
    int lastPreviewMode = 0;
    bool lastView3D = false;
    bool lastPaintMode = false;
    int lastMapResolution = params.mapResolution;

    // Undo/Redo system for paint mode (Blender-style: last 10 commands)
    std::vector<Image> undoStack;
    std::vector<Image> redoStack;
    const size_t maxUndoLevels = 10;
    bool ctrlPressed = false;

    // Viewport focus tracking
    bool last2DViewFocused = true;  // Start with 2D viewport focused

    // Async generation support
    std::future<std::pair<Image, LandscapeDesign>> generationFuture;
    std::atomic<bool> isGenerating(false);
    LandscapeParameters paramsSnapshot;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGuiIO& io = ImGui::GetIO();
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Check for Ctrl key state
        ctrlPressed = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || 
                       glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);

        // Handle Undo (Ctrl+Z)
        if (ctrlPressed && glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && !undoStack.empty()) {
            static double lastUndoTime = 0.0;
            double currentTime = glfwGetTime();
            // Debounce to prevent multiple undos from one key press
            if (currentTime - lastUndoTime > 0.2) {
                // Save current state to redo stack
                redoStack.push_back(rawHeight);
                if (redoStack.size() > maxUndoLevels) {
                    redoStack.erase(redoStack.begin());
                }

                // Restore from undo stack
                rawHeight = undoStack.back();
                undoStack.pop_back();

                // Update display
                design = LandscapeDesigner::designLandscape(rawHeight, params);
                renderer.uploadTexture(design.height);
                if (params.view3D) {
                    renderer.updateTerrainMesh(design.height, params.heightScale);
                }

                lastUndoTime = currentTime;
            }
        }

        // Handle Redo (Ctrl+Y)
        if (ctrlPressed && glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && !redoStack.empty()) {
            static double lastRedoTime = 0.0;
            double currentTime = glfwGetTime();
            // Debounce to prevent multiple redos from one key press
            if (currentTime - lastRedoTime > 0.2) {
                // Save current state to undo stack
                undoStack.push_back(rawHeight);
                if (undoStack.size() > maxUndoLevels) {
                    undoStack.erase(undoStack.begin());
                }

                // Restore from redo stack
                rawHeight = redoStack.back();
                redoStack.pop_back();

                // Update display
                design = LandscapeDesigner::designLandscape(rawHeight, params);
                renderer.uploadTexture(design.height);
                if (params.view3D) {
                    renderer.updateTerrainMesh(design.height, params.heightScale);
                }

                lastRedoTime = currentTime;
            }
        }

        // Handle Data Dump (D key) - dumps data from focused viewport
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && rawHeight.size() > 0) {
            static double lastDumpTime = 0.0;
            double currentTime = glfwGetTime();
            // Debounce to prevent multiple dumps from one key press
            if (currentTime - lastDumpTime > 0.5) {
                std::string filename;
                std::string viewportName;

                if (params.paintMode) {
                    filename = "heightmap_paintmode.txt";
                    viewportName = "Paint Mode";
                } else if (last2DViewFocused) {
                    filename = "heightmap_2dview.txt";
                    viewportName = "2D Viewport";
                } else {
                    filename = "heightmap_3dview.txt";
                    viewportName = "3D Viewport";
                }

                DataDumper::dumpHeightmap(rawHeight, filename);

                // Also dump a small sample area (center 20x20)
                int centerX = static_cast<int>(rawHeight.width()) / 2 - 10;
                int centerY = static_cast<int>(rawHeight.height()) / 2 - 10;
                std::string sampleFilename = filename.substr(0, filename.length() - 4) + "_sample.txt";
                DataDumper::dumpSample(rawHeight, sampleFilename, centerX, centerY, 20, 20);

                std::cout << "Data dumped from " << viewportName << "!" << std::endl;
                lastDumpTime = currentTime;
            }
        }

        // Handle paint mode (now handled in dual viewport rendering section below)
        // Paint mode interaction moved to viewport-specific handling

        // Apply erosion if requested
        if (params.erosionMode && rawHeight.size() > 0) {
            std::cout << "Applying erosion..." << std::endl;
            
            erosionSim.applyErosion(rawHeight, 
                                   params.hydraulicIterations,
                                   params.thermalIterations,
                                   params.rainAmount,
                                   params.evaporation,
                                   params.hydraulicStrength,
                                   params.thermalStrength,
                                   params.talusAngle);
            
            design = LandscapeDesigner::designLandscape(rawHeight, params);
            renderer.uploadTexture(design.height);

            if (params.view3D) {
                renderer.updateTerrainMesh(design.height, params.heightScale);
            }

            params.erosionMode = false;
            std::cout << "Erosion complete!" << std::endl;
        }

        // Handle terrain normalization to range
        if (params.normalizeTerrainToRange && rawHeight.size() > 0) {
            std::cout << "Normalizing terrain to range [" << params.minHeight 
                      << ", " << params.maxHeight << "]..." << std::endl;

            // Save current state to undo stack
            undoStack.push_back(rawHeight);
            if (undoStack.size() > maxUndoLevels) {
                undoStack.erase(undoStack.begin());
            }
            // Clear redo stack when new action is performed
            redoStack.clear();

            // Normalize to target range
            rawHeight = LandscapeDesigner::normalizeToRange(rawHeight, params.minHeight, params.maxHeight);

            // Update design and display
            design = LandscapeDesigner::designLandscape(rawHeight, params);

            switch (params.previewMode) {
                case 0: renderer.uploadTexture(design.height); break;
                case 1: renderer.uploadTexture(design.slope); break;
                case 2: renderer.uploadTexture(design.curvature); break;
                case 3: renderer.uploadTexture(design.biomeMask); break;
                case 4: renderer.uploadTexture(design.waterMask); break;
                default: renderer.uploadTexture(design.height); break;
            }

            if (params.view3D) {
                renderer.updateTerrainMesh(design.height, params.heightScale);
            }

            params.normalizeTerrainToRange = false;
            std::cout << "Normalization complete!" << std::endl;
        }

        // Check if async generation is complete
        if (isGenerating && generationFuture.valid()) {
            if (generationFuture.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
                auto result = generationFuture.get();
                rawHeight = std::move(result.first);
                design = std::move(result.second);
                
                switch (paramsSnapshot.previewMode) {
                    case 0: renderer.uploadTexture(design.height); break;
                    case 1: renderer.uploadTexture(design.slope); break;
                    case 2: renderer.uploadTexture(design.curvature); break;
                    case 3: renderer.uploadTexture(design.biomeMask); break;
                    case 4: renderer.uploadTexture(design.waterMask); break;
                    default: renderer.uploadTexture(design.height); break;
                }
                
                if (paramsSnapshot.view3D) {
                    renderer.updateTerrainMesh(design.height, paramsSnapshot.heightScale);
                }
                
                lastPreviewMode = paramsSnapshot.previewMode;
                lastView3D = paramsSnapshot.view3D;
                lastMapResolution = paramsSnapshot.mapResolution;
                params.needsUpdate = false;
                isGenerating = false;
            }
        }

        // Handle resolution changes with resampling to preserve paint edits
        if (params.mapResolution != lastMapResolution && rawHeight.size() > 0) {
            std::cout << "Resolution changed from " << lastMapResolution << " to " 
                      << params.mapResolution << ", resampling heightmap..." << std::endl;

            // Resample existing heightmap to new resolution
            rawHeight = resampleImage(rawHeight, params.mapResolution, params.mapResolution);

            // Clear undo/redo stacks (states are incompatible after resolution change)
            undoStack.clear();
            redoStack.clear();

            // Update design and display
            design = LandscapeDesigner::designLandscape(rawHeight, params);

            switch (params.previewMode) {
                case 0: renderer.uploadTexture(design.height); break;
                case 1: renderer.uploadTexture(design.slope); break;
                case 2: renderer.uploadTexture(design.curvature); break;
                case 3: renderer.uploadTexture(design.biomeMask); break;
                case 4: renderer.uploadTexture(design.waterMask); break;
                default: renderer.uploadTexture(design.height); break;
            }

            if (params.view3D) {
                renderer.updateTerrainMesh(design.height, params.heightScale);
            }

            lastMapResolution = params.mapResolution;
            params.needsUpdate = false;

            std::cout << "Resampling complete!" << std::endl;
        }

        // Start async generation if needed
        if (params.shouldUpdate() && !isGenerating) {
            paramsSnapshot = params;
            isGenerating = true;

            generationFuture = std::async(std::launch::async, [paramsSnapshot]() {
                Image height = generateHeightmap(paramsSnapshot);
                LandscapeDesign localDesign = LandscapeDesigner::designLandscape(height, paramsSnapshot);
                return std::make_pair(std::move(height), std::move(localDesign));
            });
        }

        // Handle paint mode entry/exit transitions
        if (params.paintMode != lastPaintMode && rawHeight.size() > 0) {
            if (params.paintMode) {
                // Entering paint mode - upload raw heightmap directly
                std::cout << "Entering paint mode - displaying raw heightmap" << std::endl;

                // Enable auto-contrast to prevent dark display
                params.autoContrastDisplay = true;

                renderer.uploadTexture(rawHeight);

                // Update design to match current raw height (for analysis layers)
                design.height = rawHeight;
                design.slope = LandscapeDesigner::computeSlope(rawHeight);
                design.curvature = LandscapeDesigner::computeCurvature(rawHeight);
                design.waterMask = LandscapeDesigner::computeWaterMask(rawHeight, params.waterLevel);
                design.biomeMask = LandscapeDesigner::computeBiomeMask(design.slope, design.waterMask, params.slopeThreshold);

                if (params.view3D) {
                    renderer.updateTerrainMesh(design.height, params.heightScale);
                }
            } else {
                // Exiting paint mode - restore proper preview mode texture
                std::cout << "Exiting paint mode - restoring preview mode" << std::endl;
                switch (params.previewMode) {
                    case 0: renderer.uploadTexture(design.height); break;
                    case 1: renderer.uploadTexture(design.slope); break;
                    case 2: renderer.uploadTexture(design.curvature); break;
                    case 3: renderer.uploadTexture(design.biomeMask); break;
                    case 4: renderer.uploadTexture(design.waterMask); break;
                    default: renderer.uploadTexture(design.height); break;
                }
            }

            lastPaintMode = params.paintMode;
        }

        // Handle preview mode changes (skip in paint mode - painting handles its own texture updates)
        if (!params.paintMode && (params.previewMode != lastPreviewMode || params.view3D != lastView3D) && design.height.size() > 0) {
            switch (params.previewMode) {
                case 0: renderer.uploadTexture(design.height); break;
                case 1: renderer.uploadTexture(design.slope); break;
                case 2: renderer.uploadTexture(design.curvature); break;
                case 3: renderer.uploadTexture(design.biomeMask); break;
                case 4: renderer.uploadTexture(design.waterMask); break;
                default: renderer.uploadTexture(design.height); break;
            }

            if (params.view3D && !lastView3D) {
                renderer.updateTerrainMesh(design.height, params.heightScale);
            }

            lastPreviewMode = params.previewMode;
            lastView3D = params.view3D;
        }

        int displayW, displayH;
        glfwGetFramebufferSize(window, &displayW, &displayH);

        // Update camera aspect ratio for 3D view (square-ish viewport)
        camera.setAspectRatio(1.0f);  // Viewports will be roughly square

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // === GLOBAL MENU BAR ===
        float menuBarHeight = 0.0f;
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Project...", "Ctrl+N")) {
                    params.showNewProjectDialog = true;
                }
                if (ImGui::MenuItem("Import Heightmap...", "Ctrl+O")) {
                    // Phase 2.1 - Heightmap Import
                    ImportResult importResult = ImageImporter::importWithDialog(params.mapResolution);

                    if (importResult.success) {
                        // Check if resolution matches current setting
                        if (importResult.width != static_cast<size_t>(params.mapResolution)) {
                            // Resample to match current resolution
                            rawHeight = resampleImage(importResult.heightmap, params.mapResolution, params.mapResolution);
                        } else {
                            rawHeight = std::move(importResult.heightmap);
                        }

                        // Clear undo/redo stacks (fresh start)
                        undoStack.clear();
                        redoStack.clear();

                        // Regenerate all analysis layers
                        design = LandscapeDesigner::designLandscape(rawHeight, params);

                        // Update renderers
                        renderer.uploadTexture(design.height);
                        if (params.view3D) {
                            renderer.updateTerrainMesh(design.height, params.heightScale);
                        }

                        std::cout << "Imported heightmap: " << importResult.width << "x" << importResult.height 
                                  << " (min: " << importResult.minValue << ", max: " << importResult.maxValue << ")\n";
                    } else if (!importResult.errorMessage.empty()) {
                        std::cerr << "Import failed: " << importResult.errorMessage << "\n";
                    }
                }
                if (ImGui::MenuItem("Export Heightmap...", "Ctrl+S")) {
                    // TODO: Export functionality
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt+F4")) {
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                bool canUndo = !undoStack.empty();
                bool canRedo = !redoStack.empty();
                if (ImGui::MenuItem("Undo", "Ctrl+Z", false, canUndo)) {
                    if (canUndo) {
                        redoStack.push_back(rawHeight);
                        if (redoStack.size() > maxUndoLevels) {
                            redoStack.erase(redoStack.begin());
                        }
                        rawHeight = undoStack.back();
                        undoStack.pop_back();
                        design = LandscapeDesigner::designLandscape(rawHeight, params);
                        renderer.uploadTexture(design.height);
                        if (params.view3D) {
                            renderer.updateTerrainMesh(design.height, params.heightScale);
                        }
                    }
                }
                if (ImGui::MenuItem("Redo", "Ctrl+Y", false, canRedo)) {
                    if (canRedo) {
                        undoStack.push_back(rawHeight);
                        if (undoStack.size() > maxUndoLevels) {
                            undoStack.erase(undoStack.begin());
                        }
                        rawHeight = redoStack.back();
                        redoStack.pop_back();
                        design = LandscapeDesigner::designLandscape(rawHeight, params);
                        renderer.uploadTexture(design.height);
                        if (params.view3D) {
                            renderer.updateTerrainMesh(design.height, params.heightScale);
                        }
                    }
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Reset Camera", "Home")) {
                    camera = Camera();  // Reset to default
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("3D Preview", nullptr, &params.view3D);
                ImGui::MenuItem("Auto-Contrast Display", nullptr, &params.autoContrastDisplay);
                ImGui::Separator();
                if (ImGui::BeginMenu("Preview Mode")) {
                    bool isHeight = params.previewMode == 0;
                    bool isSlope = params.previewMode == 1;
                    bool isCurvature = params.previewMode == 2;
                    bool isBiome = params.previewMode == 3;
                    bool isWater = params.previewMode == 4;
                    if (ImGui::MenuItem("Height", nullptr, isHeight)) params.previewMode = 0;
                    if (ImGui::MenuItem("Slope", nullptr, isSlope)) params.previewMode = 1;
                    if (ImGui::MenuItem("Curvature", nullptr, isCurvature)) params.previewMode = 2;
                    if (ImGui::MenuItem("Biome Mask", nullptr, isBiome)) params.previewMode = 3;
                    if (ImGui::MenuItem("Water Mask", nullptr, isWater)) params.previewMode = 4;
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Tools")) {
                if (ImGui::MenuItem("Apply Erosion")) {
                    params.erosionMode = true;
                }
                if (ImGui::MenuItem("Normalize Terrain to Range")) {
                    params.normalizeTerrainToRange = true;
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help")) {
                if (ImGui::MenuItem("About")) {
                    // TODO: Show about dialog
                }
                ImGui::EndMenu();
            }
            menuBarHeight = ImGui::GetWindowSize().y;
            ImGui::EndMainMenuBar();
        }

        // === MODE TOOLBAR ===
        float modeToolbarHeight = 0.0f;
        ImGui::SetNextWindowPos(ImVec2(0, menuBarHeight), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(displayW), 50), ImGuiCond_Always);
        ImGui::Begin("Mode Toolbar", nullptr, 
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

        ImGui::Text("Mode:");
        ImGui::SameLine();

        // Mode buttons with visual feedback
        if (ImGui::Button("Select", ImVec2(100, 30))) {
            params.currentMode = EditorMode::Select;
            params.paintMode = false;
        }
        if (params.currentMode == EditorMode::Select) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "<");
        }

        ImGui::SameLine();
        if (ImGui::Button("Paint", ImVec2(100, 30))) {
            params.currentMode = EditorMode::Paint;
            params.paintMode = true;
        }
        if (params.currentMode == EditorMode::Paint) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "<");
        }

        ImGui::SameLine();
        ImGui::BeginDisabled();  // Future modes - disabled for now
        if (ImGui::Button("Sculpt", ImVec2(100, 30))) {
            params.currentMode = EditorMode::Sculpt;
        }
        ImGui::SameLine();
        if (ImGui::Button("Ramp", ImVec2(100, 30))) {
            params.currentMode = EditorMode::Ramp;
        }
        ImGui::SameLine();
        if (ImGui::Button("Smooth", ImVec2(100, 30))) {
            params.currentMode = EditorMode::Smooth;
        }
        ImGui::EndDisabled();

        modeToolbarHeight = ImGui::GetWindowSize().y;
        ImGui::End();

        // === 3-PANEL LAYOUT SYSTEM ===

        // Calculate layout sizes (responsive to window size, accounting for menu bar + toolbar)
        const float toolsPanelWidth = 420.0f;
        const float viewportPadding = 10.0f;
        const float uiTopOffset = menuBarHeight + modeToolbarHeight;

        float remainingWidth = static_cast<float>(displayW) - toolsPanelWidth - (viewportPadding * 4);
        int viewportWidth = static_cast<int>(remainingWidth / 2.0f);
        int viewportHeight = displayH - static_cast<int>(viewportPadding * 2) - static_cast<int>(uiTopOffset);

        // Resize framebuffers if window size changed
        if (fbo2D.getWidth() != viewportWidth || fbo2D.getHeight() != viewportHeight) {
            fbo2D.resize(viewportWidth, viewportHeight);
            fbo3D.resize(viewportWidth, viewportHeight);
        }

        // Render to 2D framebuffer (center viewport - top-down paint view)
        fbo2D.bind();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.renderQuad(viewportWidth, viewportHeight, params.previewMode, params.autoContrastDisplay);
        fbo2D.unbind();

        // Render to 3D framebuffer (right viewport - perspective preview)
        fbo3D.bind();
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (params.view3D && design.height.size() > 0) {
            renderer.render3D(viewportWidth, viewportHeight, camera, params.previewMode, params.autoContrastDisplay);
        } else {
            // If 3D disabled, show same as 2D
            renderer.renderQuad(viewportWidth, viewportHeight, params.previewMode, params.autoContrastDisplay);
        }
        fbo3D.unbind();

        // Reset to default framebuffer for UI rendering
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, displayW, displayH);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // === TOOLS PANEL (Left Side, Always Visible) ===
        ImGui::SetNextWindowPos(ImVec2(viewportPadding, uiTopOffset + viewportPadding), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(toolsPanelWidth, static_cast<float>(viewportHeight)), ImGuiCond_Always);
        ImGui::Begin("Tools & Parameters", nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);

        UIPanel::render(params, &design, &camera);

        ImGui::End();

        // === 2D PAINT VIEW WINDOW (Center) ===
        float view2DX = toolsPanelWidth + viewportPadding * 2;
        ImGui::SetNextWindowPos(ImVec2(view2DX, uiTopOffset + viewportPadding), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(viewportWidth), static_cast<float>(viewportHeight)), ImGuiCond_Always);
        ImGui::Begin("2D Paint View", nullptr, 
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

        // Display 2D framebuffer texture
        float imageWidth = static_cast<float>(viewportWidth) - 20.0f;
        float imageHeight = static_cast<float>(viewportHeight) - 40.0f;
        ImGui::Image((void*)(intptr_t)fbo2D.getColorTexture(), 
                     ImVec2(imageWidth, imageHeight),
                     ImVec2(0, 1), ImVec2(1, 0));  // Flip Y for OpenGL texture coordinates

        // Check if mouse is over 2D viewport for painting
        bool is2DViewHovered = ImGui::IsItemHovered();
        ImVec2 mousePos2D = ImGui::GetMousePos();
        ImVec2 windowPos2D = ImGui::GetWindowPos();
        ImVec2 contentMin2D = ImVec2(windowPos2D.x + 10.0f, windowPos2D.y + 30.0f);
        ImVec2 contentMax2D = ImVec2(contentMin2D.x + imageWidth, contentMin2D.y + imageHeight);

        // Track viewport focus for D key dumps
        if (is2DViewHovered) {
            last2DViewFocused = true;
        }

        // === PAINT MODE INTERACTION ===
        if (params.paintMode && is2DViewHovered && rawHeight.size() > 0) {
            // Calculate mouse position relative to 2D viewport content area
            float relativeX = mousePos2D.x - contentMin2D.x;
            float relativeY = mousePos2D.y - contentMin2D.y;

            // Convert to normalized coordinates (0.0 to 1.0)
            float normalizedX = relativeX / imageWidth;
            float normalizedY = 1.0f - (relativeY / imageHeight);  // Flip Y for texture coords

            // Convert to heightmap coordinates
            int heightmapX = static_cast<int>(normalizedX * static_cast<float>(rawHeight.width()));
            int heightmapY = static_cast<int>(normalizedY * static_cast<float>(rawHeight.height()));

            // Apply brush on left click
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                if (heightmapX >= 0 && heightmapX < static_cast<int>(rawHeight.width()) &&
                    heightmapY >= 0 && heightmapY < static_cast<int>(rawHeight.height())) {

                    // Save state to undo stack before first paint stroke
                    if (!isPainting) {
                        undoStack.push_back(rawHeight);
                        if (undoStack.size() > maxUndoLevels) {
                            undoStack.erase(undoStack.begin());
                        }
                        // Clear redo stack when new action is performed
                        redoStack.clear();
                    }

                    brushTool.setFlattenHeight(params.flattenHeight);
                    brushTool.applyBrush(rawHeight, heightmapX, heightmapY, 
                                        params.brushType, params.brushRadius, params.brushStrength, params);

                    // Update texture directly
                    renderer.uploadTexture(rawHeight);

                    // Update design for analysis layers
                    design.height = rawHeight;
                    design.slope = LandscapeDesigner::computeSlope(rawHeight);
                    design.curvature = LandscapeDesigner::computeCurvature(rawHeight);
                    design.waterMask = LandscapeDesigner::computeWaterMask(rawHeight, params.waterLevel);
                    design.biomeMask = LandscapeDesigner::computeBiomeMask(design.slope, design.waterMask, params.slopeThreshold);

                    if (params.view3D) {
                        renderer.updateTerrainMesh(design.height, params.heightScale);
                    }

                    // Re-render framebuffers immediately after painting to show updated terrain
                    fbo2D.bind();
                    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    renderer.renderQuad(viewportWidth, viewportHeight, params.previewMode, params.autoContrastDisplay);
                    fbo2D.unbind();

                    fbo3D.bind();
                    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    if (params.view3D && design.height.size() > 0) {
                        renderer.render3D(viewportWidth, viewportHeight, camera, params.previewMode, params.autoContrastDisplay);
                    } else {
                        // If 3D disabled, show same as 2D
                        renderer.renderQuad(viewportWidth, viewportHeight, params.previewMode, params.autoContrastDisplay);
                    }
                    fbo3D.unbind();

                    // Reset to default framebuffer
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);
                    glViewport(0, 0, displayW, displayH);

                    isPainting = true;
                }
            } else {
                isPainting = false;
            }
        }

        ImGui::End();

        // === 3D PREVIEW WINDOW (Right Side) ===
        float view3DX = view2DX + static_cast<float>(viewportWidth) + viewportPadding;
        ImGui::SetNextWindowPos(ImVec2(view3DX, uiTopOffset + viewportPadding), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(viewportWidth), static_cast<float>(viewportHeight)), ImGuiCond_Always);
        ImGui::Begin("3D Preview", nullptr,
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

        // Display 3D framebuffer texture
        ImGui::Image((void*)(intptr_t)fbo3D.getColorTexture(),
                     ImVec2(imageWidth, imageHeight),
                     ImVec2(0, 1), ImVec2(1, 0));

        bool is3DViewHovered = ImGui::IsItemHovered();

        // Track viewport focus for D key dumps
        if (is3DViewHovered) {
            last2DViewFocused = false;
        }

        // === 3D CAMERA CONTROLS (Only when 3D viewport is hovered) ===
        if (is3DViewHovered && params.view3D && !params.paintMode) {
            if (!firstMouse) {
                float deltaX = static_cast<float>(xpos - lastMouseX);
                float deltaY = static_cast<float>(ypos - lastMouseY);

                // Left mouse button - rotate camera
                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                    camera.rotate(deltaX * 0.5f, -deltaY * 0.5f);
                }

                // Right mouse button - pan camera
                if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
                    camera.pan(-deltaX * 0.01f, deltaY * 0.01f);
                }
            }

            lastMouseX = xpos;
            lastMouseY = ypos;
            firstMouse = false;

            // Handle scroll wheel zoom
            if (io.MouseWheel != 0.0f) {
                camera.zoom(-io.MouseWheel * 0.2f);
            }
        } else if (!is2DViewHovered) {
            // Reset firstMouse when not hovering any viewport
            firstMouse = true;
        }

        ImGui::End();

        // === NEW PROJECT DIALOG ===
        if (params.showNewProjectDialog) {
            ImGui::OpenPopup("New Project");
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        }

        if (ImGui::BeginPopupModal("New Project", &params.showNewProjectDialog, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Create New Terrain Project");
            ImGui::Separator();
            ImGui::Spacing();

            // Engine Selection
            ImGui::Text("Target Engine:");
            const char* engines[] = { "Unreal Engine", "Unity", "Godot", "Custom" };
            int currentEngine = static_cast<int>(params.targetEngine);
            if (ImGui::Combo("##Engine", &currentEngine, engines, 4)) {
                params.targetEngine = static_cast<TargetEngine>(currentEngine);
            }
            ImGui::Spacing();

            // Terrain Type Selection (affects scale calculation)
            ImGui::Text("Terrain Type:");
            const char* terrainTypes[] = { "Plains", "Hills", "Mountains", "Coastal", "Custom" };
            int currentTerrainType = static_cast<int>(params.terrainType);
            if (ImGui::Combo("##TerrainType", &currentTerrainType, terrainTypes, 5)) {
                params.terrainType = static_cast<TerrainType>(currentTerrainType);
            }
            ImGui::Spacing();

            // Terrain Size
            ImGui::Text("Terrain World Size:");
            const char* sizes[] = { "512m", "1km", "2km", "4km", "8km" };
            static int sizeIndex = 1;  // Default 1km
            if (ImGui::Combo("##Size", &sizeIndex, sizes, 5)) {
                float sizeValues[] = { 512.0f, 1000.0f, 2000.0f, 4000.0f, 8000.0f };
                params.terrainWorldSize = sizeValues[sizeIndex];
            }
            ImGui::Spacing();

            // Starting Template
            ImGui::Text("Starting Template:");
            const char* templates[] = { "Perlin Noise (Procedural)", "Flat (Sculpt from Scratch)" };
            int currentTemplate = static_cast<int>(params.startingTemplate);
            ImGui::Combo("##Template", &currentTemplate, templates, 2);
            params.startingTemplate = static_cast<StartingTemplate>(currentTemplate);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Info text
            ImGui::TextWrapped("This will set up your project with engine-appropriate height ranges and generate the initial terrain.");

            ImGui::Spacing();

            // Buttons
            if (ImGui::Button("Create Project", ImVec2(120, 0))) {
                // Enable engine presets
                params.useEnginePresets = true;

                // Calculate appropriate height range based on engine and terrain type
                auto range = EngineScalingHelper::calculateClampRange(params);
                params.minHeight = range.first;
                params.maxHeight = range.second;
                params.enableHeightClamping = true;

                // Generate terrain based on template
                switch (params.startingTemplate) {
                    case StartingTemplate::PerlinNoise:
                        // Use existing Perlin generation (will happen automatically via needsUpdate)
                        params.markChanged();
                        break;

                    case StartingTemplate::FlatSculptable:
                        // Generate flat terrain at mid-height
                        rawHeight = generateFlatTerrain(params.mapResolution, 0.5f);
                        design = LandscapeDesigner::designLandscape(rawHeight, params);
                        renderer.uploadTexture(design.height);
                        if (params.view3D) {
                            renderer.updateTerrainMesh(design.height, params.heightScale);
                        }
                        break;
                }

                // Clear undo/redo stacks for fresh project
                undoStack.clear();
                redoStack.clear();

                // Reset to Select mode
                params.currentMode = EditorMode::Select;
                params.paintMode = false;

                std::cout << "New project created!" << std::endl;
                std::cout << "  Engine: " << engines[static_cast<int>(params.targetEngine)] << std::endl;
                std::cout << "  Terrain Type: " << terrainTypes[static_cast<int>(params.terrainType)] << std::endl;
                std::cout << "  Size: " << params.terrainWorldSize << "m" << std::endl;
                std::cout << "  Height Range: " << params.minHeight << " - " << params.maxHeight << std::endl;

                params.showNewProjectDialog = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(120, 0))) {
                params.showNewProjectDialog = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::Render();

        // Draw brush cursor overlay on 2D view when in paint mode
        if (params.paintMode && is2DViewHovered && rawHeight.size() > 0) {
            // Calculate mouse position relative to 2D viewport content area
            float relativeX = mousePos2D.x - contentMin2D.x;
            float relativeY = mousePos2D.y - contentMin2D.y;

            // Convert to normalized coordinates
            float normalizedX = relativeX / imageWidth;
            float normalizedY = 1.0f - (relativeY / imageHeight);  // Flip Y

            // Convert to heightmap coordinates
            int heightmapX = static_cast<int>(normalizedX * static_cast<float>(rawHeight.width()));
            int heightmapY = static_cast<int>(normalizedY * static_cast<float>(rawHeight.height()));

            // Draw brush cursor (rendered to default framebuffer over 2D view)
            fbo2D.bind();
            if (heightmapX >= 0 && heightmapX < static_cast<int>(rawHeight.width()) &&
                heightmapY >= 0 && heightmapY < static_cast<int>(rawHeight.height())) {

                float screenRadius = (params.brushRadius / static_cast<float>(params.mapResolution)) * imageWidth;
                renderer.drawBrushCursor(viewportWidth, viewportHeight,
                                        relativeX, 
                                        relativeY,
                                        screenRadius);
            }
            fbo2D.unbind();
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    fbo2D.cleanup();
    fbo3D.cleanup();
    renderer.cleanup();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
