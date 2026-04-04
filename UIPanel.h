#pragma once
#include "LandscapeParameters.h"
#include "LandscapeDesign.h"
#include "Camera.h"

class UIPanel {
public:
    static void render(LandscapeParameters& params, const LandscapeDesign* design = nullptr, Camera* camera = nullptr);

private:
    static void renderPreviewSelector(LandscapeParameters& params);
    static void renderParameterSliders(LandscapeParameters& params);
    static void renderStatistics(const LandscapeDesign* design);
    static void renderExportSection(LandscapeParameters& params, const LandscapeDesign* design);
    static void render3DControls(LandscapeParameters& params, Camera* camera);
    static void renderPaintModeControls(LandscapeParameters& params);
    static void renderErosionControls(LandscapeParameters& params);
};
