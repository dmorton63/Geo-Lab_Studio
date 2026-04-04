#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera();

    void setAspectRatio(float aspect);
    void setDistance(float dist);
    void rotate(float deltaYaw, float deltaPitch);
    void pan(float deltaX, float deltaY);
    void zoom(float delta);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

    void reset();

    float getDistance() const { return distance; }
    float getYaw() const { return yaw; }
    float getPitch() const { return pitch; }

private:
    void updateViewMatrix();

    float distance;
    float yaw;
    float pitch;
    glm::vec3 target;
    glm::vec3 position;

    float aspectRatio;
    float fov;
    float nearPlane;
    float farPlane;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
};
