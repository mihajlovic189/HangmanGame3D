#pragma once
#ifndef RENDERER3D_H
#define RENDERER3D_H

#include <glm/glm.hpp>
#include "model.hpp"
#include "Camera.h"

class Renderer3D {
public:
    Renderer3D();
    ~Renderer3D();
    
    void initialize(unsigned int shader3D, unsigned int shader3DTexture, unsigned int modelShader,
                   unsigned int plankTex, unsigned int plankVertTex, unsigned int ropeTex,
                   unsigned int groundTex, unsigned int landscapeTex,
                   Model* logWallModel, Model* platformModel);
    
    void drawGround3D(Camera* camera);
    void drawGallows3D(Camera* camera);
    void drawHangman3D(Camera* camera, int wrongCount);
    void drawLogWall(Camera* camera, const glm::vec3& centerPos, float radius);
    void renderPlatform(Camera* camera, const glm::vec3& position, float rotationAngle);
    void drawCylinder3D(Camera* camera, glm::vec3 position, glm::vec3 scale, unsigned int texture);
    
    void setLightPosition(const glm::vec3& pos) { lightPos = pos; }
    void setLightColor(const glm::vec3& col) { lightColor = col; }

private:
    void drawHead3D(glm::vec3 position, float radius, glm::vec3 color, float angle, glm::vec3 axis, Camera* camera);
    void drawCube3D(glm::vec3 position, glm::vec3 scale, glm::vec3 color, glm::vec3 anglesDeg, Camera* camera);
    void drawTexturedCube3D(glm::vec3 position, glm::vec3 scale, unsigned int texture, glm::vec3 anglesDeg, 
                           float tilingX, float tilingY, Camera* camera);
    
    unsigned int shader3D;
    unsigned int shader3DTexture;
    unsigned int modelShader;
    
    unsigned int plankTexture;
    unsigned int plankVertTexture;
    unsigned int ropeTexture;
    unsigned int groundTexture;
    unsigned int landscapeTexture;
    
    Model* logWall;
    Model* platform;
    
    glm::vec3 lightPos;
    glm::vec3 lightColor;
};

#endif
