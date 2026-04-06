#pragma once
#include "Image.h"
#include "Camera.h"
#include "TerrainMesh.h"
#include <glad/glad.h>
#include <glm/glm.hpp>

class Renderer {
public:
    Renderer();
    ~Renderer();

    void init();
    void cleanup();

    void uploadTexture(const Image& image);
    void renderQuad(int windowWidth, int windowHeight, int previewMode = 0, bool autoContrast = true);
    void render3D(int windowWidth, int windowHeight, const Camera& camera, int previewMode = 0, bool autoContrast = true);
    void drawBrushCursor(int windowWidth, int windowHeight, float mouseX, float mouseY, float radius);

    void updateTerrainMesh(const Image& heightmap, float heightScale = 1.0f);

    // Set the target data range for consistent 3D height scaling (used after normalization)
    void setTargetDataRange(float minVal, float maxVal) {
        targetDataRange = maxVal - minVal;
    }

    GLuint getTextureID() const { return textureID; }

private:
    GLuint textureID = 0;
    GLuint shaderProgram2D = 0;
    GLuint shaderProgram3D = 0;
    GLuint vao = 0;
    GLuint vbo = 0;

    TerrainMesh terrainMesh;

    // Display range for auto-contrast
    float displayMin = 0.0f;
    float displayMax = 1.0f;

    // Target data range for 3D height scaling (preserved from engine presets)
    // This prevents 3D height from changing when painting within the normalized range
    float targetDataRange = 1.0f;  // Default to full range, updated when normalized terrain is uploaded

    GLuint compileShader(const char* source, GLenum type);
    GLuint createShaderProgram2D();
    GLuint createShaderProgram3D();
};
