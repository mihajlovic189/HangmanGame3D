#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;
    
    float orbitAngle;
    float orbitDistance;
    
    Camera(float aspect = 16.0f / 9.0f);
    
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
 
    void updateOrbit(float angleOffset);
    void setOrbitDistance(float distance);
    
private:
    void updatePosition();
};

#endif
