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
    void renderQuad(int windowWidth, int windowHeight, int previewMode = 0);
    void render3D(int windowWidth, int windowHeight, const Camera& camera, int previewMode = 0);
    void drawBrushCursor(int windowWidth, int windowHeight, float mouseX, float mouseY, float radius);

    void updateTerrainMesh(const Image& heightmap, float heightScale = 1.0f);

    GLuint getTextureID() const { return textureID; }

private:
    GLuint textureID = 0;
    GLuint shaderProgram2D = 0;
    GLuint shaderProgram3D = 0;
    GLuint vao = 0;
    GLuint vbo = 0;

    TerrainMesh terrainMesh;

    GLuint compileShader(const char* source, GLenum type);
    GLuint createShaderProgram2D();
    GLuint createShaderProgram3D();
};
