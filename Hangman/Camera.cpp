#include "Camera.h"
#include <cmath>

Camera::Camera(float aspect) 
    : position(0.0f, 0.0f, 3.0f),
      target(0.0f, 0.0f, 0.0f),
      up(0.0f, 1.0f, 0.0f),
      fov(45.0f),
      aspectRatio(aspect),
      nearPlane(0.1f),
      farPlane(100.0f),
      orbitAngle(0.0f),
      orbitDistance(3.0f)
{
    updatePosition();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

void Camera::updateOrbit(float angleOffset) {
    orbitAngle += angleOffset;
    updatePosition();
}

void Camera::setOrbitDistance(float distance) {
    orbitDistance = distance;
    updatePosition();
}

void Camera::updatePosition() {
    float x = orbitDistance * std::sin(glm::radians(orbitAngle));
    float z = orbitDistance * std::cos(glm::radians(orbitAngle));
    position = glm::vec3(x, 0.5f, z);
}
