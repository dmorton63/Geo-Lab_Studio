#include "Renderer.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

const char* vertexShaderSource2D = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    TexCoord = aTexCoord;
}
)";

const char* fragmentShaderSource2D = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D heightTexture;
uniform int previewMode;
uniform int autoContrast;
uniform float displayMin;
uniform float displayMax;

vec3 heatmap(float t) {
    // Blue -> Cyan -> Green -> Yellow -> Red
    t = clamp(t, 0.0, 1.0);

    vec3 color;
    if (t < 0.25) {
        color = mix(vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 1.0), t * 4.0);
    }
    else if (t < 0.5) {
        color = mix(vec3(0.0, 1.0, 1.0), vec3(0.0, 1.0, 0.0), (t - 0.25) * 4.0);
    }
    else if (t < 0.75) {
        color = mix(vec3(0.0, 1.0, 0.0), vec3(1.0, 1.0, 0.0), (t - 0.5) * 4.0);
    }
    else {
        color = mix(vec3(1.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0), (t - 0.75) * 4.0);
    }

    return color;
}

void main() {
    float value = texture(heightTexture, TexCoord).r;

    // Apply auto-contrast if enabled (display only, doesn't modify data)
    if (autoContrast == 1 && previewMode == 0) {
        value = (value - displayMin) / (displayMax - displayMin);
        value = clamp(value, 0.0, 1.0);
    }

    if (previewMode == 0) {
        // Height - Grayscale (with optional auto-contrast)
        FragColor = vec4(value, value, value, 1.0);
    }
    else if (previewMode == 1) {
        // Slope - Heatmap (scale up for visibility)
        // Slope values are typically 0.0 to 0.3, so scale by 5x
        float scaled = clamp(value * 5.0, 0.0, 1.0);
        vec3 color = heatmap(scaled);
        FragColor = vec4(color, 1.0);
    }
    else if (previewMode == 2) {
        // Curvature - Heatmap centered at 0
        // Scale and center around 0.5
        float scaled = abs((value - 0.5) * 10.0);
        scaled = clamp(scaled, 0.0, 1.0);
        vec3 color = heatmap(scaled);
        FragColor = vec4(color, 1.0);
    }
    else if (previewMode == 3) {
        // Biome - Color-coded
        int biomeType = int(value + 0.5);
        vec3 color;

        if (biomeType == 0) {
            // Water - Blue
            color = vec3(0.2, 0.5, 0.9);
        }
        else if (biomeType == 1) {
            // Grass - Green
            color = vec3(0.3, 0.7, 0.3);
        }
        else {
            // Rock - Gray
            color = vec3(0.6, 0.6, 0.6);
        }

        FragColor = vec4(color, 1.0);
    }
    else if (previewMode == 4) {
        // Water Mask - Blue/Black
        vec3 color = value > 0.5 ? vec3(0.2, 0.5, 0.9) : vec3(0.0, 0.0, 0.0);
        FragColor = vec4(color, 1.0);
    }
    else {
        // Default - Grayscale
        FragColor = vec4(value, value, value, 1.0);
    }
}
)";

const char* vertexShaderSource3D = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out float Height;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    Height = aPos.y;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource3D = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in float Height;

uniform sampler2D heightTexture;
uniform int previewMode;
uniform vec3 lightDir;
uniform int autoContrast;
uniform float displayMin;
uniform float displayMax;

vec3 heatmap(float t) {
    t = clamp(t, 0.0, 1.0);
    vec3 color;
    if (t < 0.25) {
        color = mix(vec3(0.0, 0.0, 1.0), vec3(0.0, 1.0, 1.0), t * 4.0);
    }
    else if (t < 0.5) {
        color = mix(vec3(0.0, 1.0, 1.0), vec3(0.0, 1.0, 0.0), (t - 0.25) * 4.0);
    }
    else if (t < 0.75) {
        color = mix(vec3(0.0, 1.0, 0.0), vec3(1.0, 1.0, 0.0), (t - 0.5) * 4.0);
    }
    else {
        color = mix(vec3(1.0, 1.0, 0.0), vec3(1.0, 0.0, 0.0), (t - 0.75) * 4.0);
    }
    return color;
}

void main() {
    float value = texture(heightTexture, TexCoord).r;

    // Apply auto-contrast if enabled (display only, doesn't modify data)
    if (autoContrast == 1 && previewMode == 0) {
        value = (value - displayMin) / (displayMax - displayMin);
        value = clamp(value, 0.0, 1.0);
    }

    vec3 baseColor;

    if (previewMode == 0) {
        baseColor = vec3(value);
    }
    else if (previewMode == 1) {
        float scaled = clamp(value * 5.0, 0.0, 1.0);
        baseColor = heatmap(scaled);
    }
    else if (previewMode == 2) {
        float scaled = abs((value - 0.5) * 10.0);
        scaled = clamp(scaled, 0.0, 1.0);
        baseColor = heatmap(scaled);
    }
    else if (previewMode == 3) {
        int biomeType = int(value + 0.5);
        if (biomeType == 0) {
            baseColor = vec3(0.2, 0.5, 0.9);
        }
        else if (biomeType == 1) {
            baseColor = vec3(0.3, 0.7, 0.3);
        }
        else {
            baseColor = vec3(0.6, 0.6, 0.6);
        }
    }
    else if (previewMode == 4) {
        baseColor = value > 0.5 ? vec3(0.2, 0.5, 0.9) : vec3(0.0, 0.0, 0.0);
    }
    else {
        baseColor = vec3(value);
    }

    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(lightDir);

    float ambient = 0.3;
    float diffuse = max(dot(norm, lightDirection), 0.0) * 0.7;

    vec3 lighting = (ambient + diffuse) * baseColor;

    FragColor = vec4(lighting, 1.0);
}
)";

Renderer::Renderer() {}

Renderer::~Renderer() {
    cleanup();
}

void Renderer::init() {
    glEnable(GL_DEPTH_TEST);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    shaderProgram2D = createShaderProgram2D();
    shaderProgram3D = createShaderProgram3D();

    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void Renderer::cleanup() {
    terrainMesh.cleanup();
    if (vao) glDeleteVertexArrays(1, &vao);
    if (vbo) glDeleteBuffers(1, &vbo);
    if (shaderProgram2D) glDeleteProgram(shaderProgram2D);
    if (shaderProgram3D) glDeleteProgram(shaderProgram3D);
    if (textureID) glDeleteTextures(1, &textureID);
}

void Renderer::uploadTexture(const Image& image) {
    // Calculate min/max for auto-contrast display
    if (image.size() > 0) {
        displayMin = image.data()[0];
        displayMax = image.data()[0];

        for (const auto& val : image.data()) {
            if (val < displayMin) displayMin = val;
            if (val > displayMax) displayMax = val;
        }

        // Avoid division by zero
        if (std::abs(displayMax - displayMin) < 0.001f) {
            displayMin = 0.0f;
            displayMax = 1.0f;
        }
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 
                 static_cast<GLsizei>(image.width()), 
                 static_cast<GLsizei>(image.height()), 
                 0, GL_RED, GL_FLOAT, image.data().data());
}

void Renderer::renderQuad(int windowWidth, int windowHeight, int previewMode, bool autoContrast) {
    glViewport(0, 0, windowWidth, windowHeight);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram2D);

    GLint previewModeLocation = glGetUniformLocation(shaderProgram2D, "previewMode");
    glUniform1i(previewModeLocation, previewMode);

    // Pass auto-contrast parameters to shader
    GLint autoContrastLoc = glGetUniformLocation(shaderProgram2D, "autoContrast");
    GLint displayMinLoc = glGetUniformLocation(shaderProgram2D, "displayMin");
    GLint displayMaxLoc = glGetUniformLocation(shaderProgram2D, "displayMax");
    glUniform1i(autoContrastLoc, autoContrast ? 1 : 0);
    glUniform1f(displayMinLoc, displayMin);
    glUniform1f(displayMaxLoc, displayMax);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Renderer::render3D(int windowWidth, int windowHeight, const Camera& camera, int previewMode, bool autoContrast) {
    glViewport(0, 0, windowWidth, windowHeight);
    glClearColor(0.15f, 0.15f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!terrainMesh.isReady()) {
        return;
    }

    glUseProgram(shaderProgram3D);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();

    GLint modelLoc = glGetUniformLocation(shaderProgram3D, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram3D, "view");
    GLint projectionLoc = glGetUniformLocation(shaderProgram3D, "projection");
    GLint previewModeLoc = glGetUniformLocation(shaderProgram3D, "previewMode");
    GLint lightDirLoc = glGetUniformLocation(shaderProgram3D, "lightDir");

    // Pass auto-contrast parameters to 3D shader
    GLint autoContrastLoc = glGetUniformLocation(shaderProgram3D, "autoContrast");
    GLint displayMinLoc = glGetUniformLocation(shaderProgram3D, "displayMin");
    GLint displayMaxLoc = glGetUniformLocation(shaderProgram3D, "displayMax");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform1i(previewModeLoc, previewMode);
    glUniform3f(lightDirLoc, 0.3f, 0.8f, 0.5f);
    glUniform1i(autoContrastLoc, autoContrast ? 1 : 0);
    glUniform1f(displayMinLoc, displayMin);
    glUniform1f(displayMaxLoc, displayMax);

    glBindTexture(GL_TEXTURE_2D, textureID);
    terrainMesh.bind();
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(terrainMesh.getIndexCount()), GL_UNSIGNED_INT, 0);
    terrainMesh.unbind();
}

void Renderer::updateTerrainMesh(const Image& heightmap, float heightScale) {
    // Auto-boost height scale for narrow ranges (like engine presets)
    // If the data range is less than 0.5, scale it up proportionally
    float dataRange = displayMax - displayMin;
    float effectiveHeightScale = heightScale;

    if (dataRange < 0.5f && dataRange > 0.001f) {
        // Boost the height scale to compensate for narrow data range
        // Target effective range of 0.5 (reasonable 3D height)
        effectiveHeightScale = heightScale * (0.5f / dataRange);

        // Cap the boost to avoid extreme values
        effectiveHeightScale = std::min(effectiveHeightScale, heightScale * 5.0f);
    }

    terrainMesh.generateFromHeightmap(heightmap, effectiveHeightScale);
}


void Renderer::drawBrushCursor(int windowWidth, int windowHeight, float mouseX, float mouseY, float radius) {
    // Draw circle using immediate mode-style rendering with line loop
    glDisable(GL_DEPTH_TEST);
    glUseProgram(0); // Use fixed-function pipeline for simple circle

    // Set up orthographic projection for 2D overlay
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, windowWidth, windowHeight, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Draw circle
    glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);

    const int segments = 32;
    for (int i = 0; i < segments; ++i) {
        float angle = (static_cast<float>(i) / segments) * 2.0f * 3.14159f;
        float x = mouseX + radius * std::cos(angle);
        float y = mouseY + radius * std::sin(angle);
        glVertex2f(x, y);
    }

    glEnd();

    // Restore matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
}

GLuint Renderer::compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error: " << infoLog << std::endl;
    }

    return shader;
}

GLuint Renderer::createShaderProgram2D() {
    GLuint vertexShader = compileShader(vertexShaderSource2D, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSource2D, GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader linking error: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

GLuint Renderer::createShaderProgram3D() {
    GLuint vertexShader = compileShader(vertexShaderSource3D, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSource3D, GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "3D Shader linking error: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}
