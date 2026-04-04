#include "TerrainMesh.h"
#include <glm/glm.hpp>
#include <iostream>

TerrainMesh::TerrainMesh() {}

TerrainMesh::~TerrainMesh() {
    cleanup();
}

void TerrainMesh::generateFromHeightmap(const Image& heightmap, float heightScale) {
    cleanup();

    size_t width = heightmap.width();
    size_t height = heightmap.height();

    if (width == 0 || height == 0) {
        std::cerr << "Invalid heightmap dimensions" << std::endl;
        return;
    }

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    vertices.reserve(width * height);

    float xScale = 2.0f / static_cast<float>(width - 1);
    float zScale = 2.0f / static_cast<float>(height - 1);

    for (size_t z = 0; z < height; ++z) {
        for (size_t x = 0; x < width; ++x) {
            Vertex vertex;

            float xPos = 1.0f - static_cast<float>(x) * xScale;
            float zPos = static_cast<float>(z) * zScale - 1.0f;
            float yPos = heightmap(x, z) * heightScale;

            vertex.position[0] = xPos;
            vertex.position[1] = yPos;
            vertex.position[2] = zPos;

            vertex.normal[0] = 0.0f;
            vertex.normal[1] = 1.0f;
            vertex.normal[2] = 0.0f;

            vertex.texCoord[0] = static_cast<float>(x) / static_cast<float>(width - 1);
            vertex.texCoord[1] = static_cast<float>(z) / static_cast<float>(height - 1);

            vertices.push_back(vertex);
        }
    }

    for (size_t z = 0; z < height - 1; ++z) {
        for (size_t x = 0; x < width - 1; ++x) {
            unsigned int topLeft = z * width + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * width + x;
            unsigned int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    calculateNormals(vertices, indices);

    indexCount = indices.size();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    glBindVertexArray(0);
}

void TerrainMesh::cleanup() {
    if (vao) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    if (vbo) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (ebo) {
        glDeleteBuffers(1, &ebo);
        ebo = 0;
    }
    indexCount = 0;
}

void TerrainMesh::bind() const {
    glBindVertexArray(vao);
}

void TerrainMesh::unbind() const {
    glBindVertexArray(0);
}

void TerrainMesh::calculateNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    for (size_t i = 0; i < vertices.size(); ++i) {
        vertices[i].normal[0] = 0.0f;
        vertices[i].normal[1] = 0.0f;
        vertices[i].normal[2] = 0.0f;
    }

    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int idx0 = indices[i];
        unsigned int idx1 = indices[i + 1];
        unsigned int idx2 = indices[i + 2];

        glm::vec3 v0(vertices[idx0].position[0], vertices[idx0].position[1], vertices[idx0].position[2]);
        glm::vec3 v1(vertices[idx1].position[0], vertices[idx1].position[1], vertices[idx1].position[2]);
        glm::vec3 v2(vertices[idx2].position[0], vertices[idx2].position[1], vertices[idx2].position[2]);

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 normal = glm::normalize(glm::cross(edge2, edge1));

        for (int j = 0; j < 3; ++j) {
            vertices[indices[i + j]].normal[0] += normal.x;
            vertices[indices[i + j]].normal[1] += normal.y;
            vertices[indices[i + j]].normal[2] += normal.z;
        }
    }

    for (size_t i = 0; i < vertices.size(); ++i) {
        glm::vec3 n(vertices[i].normal[0], vertices[i].normal[1], vertices[i].normal[2]);
        n = glm::normalize(n);
        vertices[i].normal[0] = n.x;
        vertices[i].normal[1] = n.y;
        vertices[i].normal[2] = n.z;
    }
}
