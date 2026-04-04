#include "LandscapeDesigner.h"
#include "LandscapeParameters.h"
#include "PerlinNoise.h"
#include "Renderer.h"
#include "UIPanel.h"
#include "Camera.h"
#include "BrushTool.h"
#include "ErosionSimulator.h"
#include "DataDumper.h"

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

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Landscape Designer V0.5 - Complete Edition", nullptr, nullptr);
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
    int lastMapResolution = params.mapResolution;

    // Undo/Redo system for paint mode (Blender-style: last 10 commands)
    std::vector<Image> undoStack;
    std::vector<Image> redoStack;
    const size_t maxUndoLevels = 10;
    bool ctrlPressed = false;

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

        // Handle Data Dump (D key) - for debugging coordinate issues
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && rawHeight.size() > 0) {
            static double lastDumpTime = 0.0;
            double currentTime = glfwGetTime();
            // Debounce to prevent multiple dumps from one key press
            if (currentTime - lastDumpTime > 0.5) {
                std::string filename;
                if (params.paintMode) {
                    filename = "heightmap_paintmode.txt";
                    DataDumper::dumpHeightmap(rawHeight, filename);
                } else if (params.view3D) {
                    filename = "heightmap_3dview.txt";
                    DataDumper::dumpHeightmap(rawHeight, filename);
                } else {
                    filename = "heightmap_2dview.txt";
                    DataDumper::dumpHeightmap(rawHeight, filename);
                }

                // Also dump a small sample area (center 20x20)
                int centerX = rawHeight.width() / 2 - 10;
                int centerY = rawHeight.height() / 2 - 10;
                std::string sampleFilename = filename.substr(0, filename.length() - 4) + "_sample.txt";
                DataDumper::dumpSample(rawHeight, sampleFilename, centerX, centerY, 20, 20);

                std::cout << "Data dumped! Mode: " << (params.paintMode ? "Paint" : params.view3D ? "3D" : "2D") << std::endl;
                lastDumpTime = currentTime;
            }
        }

        // Handle paint mode
        if (!io.WantCaptureMouse && params.paintMode && rawHeight.size() > 0) {
            int displayW, displayH;
            glfwGetFramebufferSize(window, &displayW, &displayH);

            // Convert screen coordinates to heightmap coordinates (invert Y for texture coordinate system)
            float normalizedX = static_cast<float>(xpos) / static_cast<float>(displayW);
            float normalizedY = 1.0f - (static_cast<float>(ypos) / static_cast<float>(displayH));

            int heightmapX = static_cast<int>(normalizedX * rawHeight.width());
            int heightmapY = static_cast<int>(normalizedY * rawHeight.height());

            // Apply brush on left click
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
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
                                        params.brushType, params.brushRadius, params.brushStrength);

                    // Update texture directly without re-normalizing (paint mode uses raw heightmap)
                    renderer.uploadTexture(rawHeight);

                    // Update design for analysis layers (slope, curvature, etc.) but don't normalize height
                    design.height = rawHeight;
                    design.slope = LandscapeDesigner::computeSlope(rawHeight);
                    design.curvature = LandscapeDesigner::computeCurvature(rawHeight);
                    design.waterMask = LandscapeDesigner::computeWaterMask(rawHeight, params.waterLevel);
                    design.biomeMask = LandscapeDesigner::computeBiomeMask(design.slope, design.waterMask, params.slopeThreshold);

                    if (params.view3D) {
                        renderer.updateTerrainMesh(design.height, params.heightScale);
                    }

                    isPainting = true;
                }
            } else {
                isPainting = false;
            }
        }

        // Handle 3D camera controls with mouse (only if not interacting with UI)
        if (!io.WantCaptureMouse && params.view3D && !params.paintMode) {
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
        } else {
            firstMouse = true;
        }

        // Handle scroll wheel zoom (only if not interacting with UI)
        if (!io.WantCaptureMouse && params.view3D && !params.paintMode && io.MouseWheel != 0.0f) {
            camera.zoom(-io.MouseWheel * 0.2f);
        }

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

        // Handle preview mode changes
        if ((params.previewMode != lastPreviewMode || params.view3D != lastView3D) && design.height.size() > 0) {
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
        camera.setAspectRatio(static_cast<float>(displayW) / static_cast<float>(displayH));

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        UIPanel::render(params, &design, &camera);

        ImGui::Render();

        if (params.view3D) {
            renderer.render3D(displayW, displayH, camera, params.previewMode);
        } else {
            renderer.renderQuad(displayW, displayH, params.previewMode);
        }

        // Draw brush cursor in paint mode
        if (params.paintMode && !io.WantCaptureMouse) {
            // Convert brush radius from heightmap space to screen space
            float screenRadius = (params.brushRadius / static_cast<float>(params.mapResolution)) * static_cast<float>(displayW);
            renderer.drawBrushCursor(displayW, displayH, 
                                    static_cast<float>(xpos), 
                                    static_cast<float>(ypos), 
                                    screenRadius);
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    renderer.cleanup();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
