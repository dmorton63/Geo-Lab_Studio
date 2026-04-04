#include "Camera.h"
#include <algorithm>
#include <glm/gtc/constants.hpp>

Camera::Camera()
    : distance(3.0f)
    , yaw(180.0f)
    , pitch(45.0f)
    , target(0.0f, 0.0f, 0.0f)
    , aspectRatio(16.0f / 9.0f)
    , fov(45.0f)
    , nearPlane(0.1f)
    , farPlane(100.0f)
{
    updateViewMatrix();
    projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

void Camera::setAspectRatio(float aspect) {
    aspectRatio = aspect;
    projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

void Camera::setDistance(float dist) {
    distance = std::clamp(dist, 1.0f, 20.0f);
    updateViewMatrix();
}

void Camera::rotate(float deltaYaw, float deltaPitch) {
    yaw += deltaYaw;
    pitch += deltaPitch;

    while (yaw > 360.0f) yaw -= 360.0f;
    while (yaw < 0.0f) yaw += 360.0f;

    pitch = std::clamp(pitch, -89.0f, 89.0f);

    updateViewMatrix();
}

void Camera::pan(float deltaX, float deltaY) {
    glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), position - target));
    glm::vec3 up = glm::normalize(glm::cross(position - target, right));

    target += right * deltaX * distance * 0.1f;
    target += up * deltaY * distance * 0.1f;

    updateViewMatrix();
}

void Camera::zoom(float delta) {
    distance += delta;
    distance = std::clamp(distance, 1.0f, 20.0f);
    updateViewMatrix();
}

glm::mat4 Camera::getViewMatrix() const {
    return viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix() const {
    return projectionMatrix;
}

void Camera::reset() {
    distance = 3.0f;
    yaw = 180.0f;
    pitch = 45.0f;
    target = glm::vec3(0.0f, 0.0f, 0.0f);
    updateViewMatrix();
}

void Camera::updateViewMatrix() {
    float yawRad = glm::radians(yaw);
    float pitchRad = glm::radians(pitch);

    position.x = target.x + distance * cos(pitchRad) * sin(yawRad);
    position.y = target.y + distance * sin(pitchRad);
    position.z = target.z + distance * cos(pitchRad) * cos(yawRad);

    viewMatrix = glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
}
