#pragma once
#include "Image.h"
#include <glad/glad.h>
#include <vector>

struct Vertex {
    float position[3];
    float normal[3];
    float texCoord[2];
};

class TerrainMesh {
public:
    TerrainMesh();
    ~TerrainMesh();

    void generateFromHeightmap(const Image& heightmap, float heightScale = 1.0f);
    void cleanup();

    void bind() const;
    void unbind() const;

    size_t getIndexCount() const { return indexCount; }
    bool isReady() const { return vao != 0; }

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    size_t indexCount = 0;

    void calculateNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
};
