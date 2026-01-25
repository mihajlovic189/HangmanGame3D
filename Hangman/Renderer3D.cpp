#include "Renderer3D.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include "Primitives3D.h"

Renderer3D::Renderer3D()
    : shader3D(0), shader3DTexture(0), modelShader(0),
      plankTexture(0), plankVertTexture(0), ropeTexture(0),
      groundTexture(0), landscapeTexture(0),
      logWall(nullptr), platform(nullptr),
      lightPos(2.0f, 3.0f, 2.0f), lightColor(1.0f, 1.0f, 1.0f) {
}

Renderer3D::~Renderer3D() {
}

void Renderer3D::initialize(unsigned int shader3D, unsigned int shader3DTexture, unsigned int modelShader,
                            unsigned int plankTex, unsigned int plankVertTex, unsigned int ropeTex,
                            unsigned int groundTex, unsigned int landscapeTex,
                            Model* logWallModel, Model* platformModel) {
    this->shader3D = shader3D;
    this->shader3DTexture = shader3DTexture;
    this->modelShader = modelShader;
    this->plankTexture = plankTex;
    this->plankVertTexture = plankVertTex;
    this->ropeTexture = ropeTex;
    this->groundTexture = groundTex;
    this->landscapeTexture = landscapeTex;
    this->logWall = logWallModel;
    this->platform = platformModel;
}

void Renderer3D::drawHead3D(glm::vec3 position, float radius, glm::vec3 color, float angle, glm::vec3 axis, Camera* camera) {
    if (!shader3D || !camera)
        return;

    glUseProgram(shader3D);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, angle, axis);
    model = glm::scale(model, glm::vec3(radius));

    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();

    glUniformMatrix4fv(glGetUniformLocation(shader3D, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader3D, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader3D, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(shader3D, "uColor"), 1, glm::value_ptr(color));
    glUniform3fv(glGetUniformLocation(shader3D, "uLightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shader3D, "uLightColor"), 1, glm::value_ptr(lightColor));
    glUniform3fv(glGetUniformLocation(shader3D, "uViewPos"), 1, glm::value_ptr(camera->position));

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
}

void Renderer3D::drawCube3D(glm::vec3 position, glm::vec3 scale, glm::vec3 color, glm::vec3 anglesDeg, Camera* camera) {
    if (!shader3D || !camera)
        return;

    glUseProgram(shader3D);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(anglesDeg.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(anglesDeg.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(anglesDeg.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, scale);

    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();

    glUniformMatrix4fv(glGetUniformLocation(shader3D, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader3D, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader3D, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));

    glUniform3fv(glGetUniformLocation(shader3D, "uColor"), 1, glm::value_ptr(color));
    glUniform3fv(glGetUniformLocation(shader3D, "uLightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shader3D, "uLightColor"), 1, glm::value_ptr(lightColor));
    glUniform3fv(glGetUniformLocation(shader3D, "uViewPos"), 1, glm::value_ptr(camera->position));

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void Renderer3D::drawTexturedCube3D(glm::vec3 position, glm::vec3 scale, unsigned int texture, 
                                   glm::vec3 anglesDeg, float tilingX, float tilingY, Camera* camera) {
    if (!shader3DTexture || !camera || !texture)
        return;

    glUseProgram(shader3DTexture);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(anglesDeg.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(anglesDeg.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(anglesDeg.z), glm::vec3(0, 0, 1));
    model = glm::scale(model, scale);

    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();

    glUniformMatrix4fv(glGetUniformLocation(shader3DTexture, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader3DTexture, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader3DTexture, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));

    glUniform3fv(glGetUniformLocation(shader3DTexture, "uLightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shader3DTexture, "uLightColor"), 1, glm::value_ptr(lightColor));
    glUniform3fv(glGetUniformLocation(shader3DTexture, "uViewPos"), 1, glm::value_ptr(camera->position));

    setTextureTiling(shader3DTexture, tilingX, tilingY);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shader3DTexture, "uTexture"), 0);

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void Renderer3D::drawGround3D(Camera* camera) {
    if (!shader3DTexture || !camera || !groundTexture)
        return;

    glUseProgram(shader3DTexture);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.3f, 0.0f));
    model = glm::scale(model, glm::vec3(15.0f, 1.0f, 15.0f));

    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();

    glUniformMatrix4fv(glGetUniformLocation(shader3DTexture, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader3DTexture, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader3DTexture, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));

    glUniform3fv(glGetUniformLocation(shader3DTexture, "uLightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shader3DTexture, "uLightColor"), 1, glm::value_ptr(lightColor));
    glUniform3fv(glGetUniformLocation(shader3DTexture, "uViewPos"), 1, glm::value_ptr(camera->position));

    setTextureTiling(shader3DTexture, 10.0f, 10.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, groundTexture);
    glUniform1i(glGetUniformLocation(shader3DTexture, "uTexture"), 0);

    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void Renderer3D::drawGallows3D(Camera* camera) {
    if (!shader3DTexture || !camera || !plankTexture)
        return;

    const float tiling = 0.1f;

    drawTexturedCube3D(glm::vec3(-0.65f, -0.2f, 0.0f), glm::vec3(0.1f, 2.2f, 0.1f),
                      plankVertTexture, glm::vec3(0.0f, 0.0f, 0.0f), tiling, tiling, camera);

    drawTexturedCube3D(glm::vec3(-0.2f, 0.95f, 0.0f), glm::vec3(1.0f, 0.1f, 0.1f),
                      plankTexture, glm::vec3(0.0f, 0.0f, 0.0f), tiling, tiling, camera);

    drawTexturedCube3D(glm::vec3(0.2f, 0.7f, 0.0f), glm::vec3(0.02f, 0.4f, 0.02f),
                      ropeTexture, glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 9.0f, camera);
}

void Renderer3D::drawHangman3D(Camera* camera, int wrongCount) {
    if (!shader3DTexture || !camera || !shader3D)
        return;

    glm::vec3 bodyColor(1.0f, 0.8f, 0.7f);
    glm::vec3 tshirtColor(0.9f, 0.6f, 0.0f);
    glm::vec3 pantsColor(0.1f, 0.3f, 0.8f);

    float baseY = 0.5f;

    if (wrongCount >= 1)
        drawHead3D(glm::vec3(0.2f, baseY + 0.05f, 0.12f), 0.2f, bodyColor, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), camera);

    if (wrongCount >= 2) {
        drawCube3D(glm::vec3(0.2f, baseY - 0.05f, 0.12f), glm::vec3(0.05f, 0.05f, 0.05f), bodyColor, glm::vec3(0.0f, 0.0f, 0.0f), camera);
        drawCube3D(glm::vec3(0.2f, baseY - 0.25f, 0.12f), glm::vec3(0.25f, 0.35f, 0.1f), tshirtColor, glm::vec3(0.0f, 0.0f, 0.0f), camera);
        drawTexturedCube3D(glm::vec3(0.2f, baseY - 0.027f, 0.041f), glm::vec3(0.02f, 0.115f, 0.02f), ropeTexture, glm::vec3(-52.5f, 0.0f, 0.0f), 0.5f, 3.0f, camera);
        drawTexturedCube3D(glm::vec3(0.2f, baseY - 0.06f, 0.12f), glm::vec3(0.08f, 0.02f, 0.08f), ropeTexture, glm::vec3(0.0f, 0.0f, 0.0f), 5.0f, 0.1f, camera);
    }

    if (wrongCount >= 3) {
        drawCube3D(glm::vec3(0.365f, baseY - 0.125f, 0.12f), glm::vec3(0.09f, 0.09f, 0.09f), tshirtColor, glm::vec3(0.0f, 0.0f, 0.0f), camera);
        drawCube3D(glm::vec3(0.365f, baseY - 0.285f, 0.12f), glm::vec3(0.05f, 0.25f, 0.05f), bodyColor, glm::vec3(0.0f, 0.0f, 0.0f), camera);
    }

    if (wrongCount >= 4) {
        drawCube3D(glm::vec3(0.045f, baseY - 0.125f, 0.12f), glm::vec3(0.09f, 0.09f, 0.09f), tshirtColor, glm::vec3(0.0f, 0.0f, 0.0f), camera);
        drawCube3D(glm::vec3(0.045f, baseY - 0.285f, 0.12f), glm::vec3(0.05f, 0.25f, 0.05f), bodyColor, glm::vec3(0.0f, 0.0f, 0.0f), camera);
    }

    if (wrongCount >= 5) {
        drawCube3D(glm::vec3(0.265f, baseY - 0.575f, 0.12f), glm::vec3(0.09f, 0.3f, 0.09f), pantsColor, glm::vec3(0.0f, 0.0f, 0.0f), camera);
        drawCube3D(glm::vec3(0.265f, baseY - 0.745f, 0.155f), glm::vec3(0.065f, 0.035f, 0.14f), bodyColor, glm::vec3(0.0f, 0.0f, 0.0f), camera);
    }

    if (wrongCount >= 6) {
        drawCube3D(glm::vec3(0.135f, baseY - 0.575f, 0.12f), glm::vec3(0.09f, 0.3f, 0.09f), pantsColor, glm::vec3(0.0f, 0.0f, 0.0f), camera);
        drawCube3D(glm::vec3(0.135f, baseY - 0.745f, 0.155f), glm::vec3(0.065f, 0.035f, 0.14f), bodyColor, glm::vec3(0.0f, 0.0f, 0.0f), camera);
    }

    if (wrongCount >= 7) {
        drawCube3D(glm::vec3(0.23f, baseY + 0.09f, 0.195f), glm::vec3(0.03f, 0.01f, 0.03f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.0f, 20.0f, 0.0f), camera);
        drawCube3D(glm::vec3(0.17f, baseY + 0.09f, 0.195f), glm::vec3(0.03f, 0.01f, 0.03f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.0f, -20.0f, 0.0f), camera);
        drawCube3D(glm::vec3(0.23f, baseY + 0.07f, 0.2095f), glm::vec3(0.03f, 0.02f, 0.01f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 15.0f, 0.0f), camera);
        drawCube3D(glm::vec3(0.23f, baseY + 0.07f, 0.2105f), glm::vec3(0.01f, 0.01f, 0.01f), glm::vec3(0.1f, 0.3f, 0.8f), glm::vec3(0.0f, 0.0f, 0.0f), camera);
        drawCube3D(glm::vec3(0.17f, baseY + 0.07f, 0.2095f), glm::vec3(0.03f, 0.02f, 0.01f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, -15.0f, 0.0f), camera);
        drawCube3D(glm::vec3(0.17f, baseY + 0.07f, 0.2105f), glm::vec3(0.01f, 0.01f, 0.01f), glm::vec3(0.1f, 0.3f, 0.8f), glm::vec3(0.0f, 0.0f, 0.0f), camera);
    }

    if (wrongCount >= 8) {
        drawCube3D(glm::vec3(0.2f, baseY + 0.05f, 0.21f), glm::vec3(0.03f, 0.05f, 0.03f), bodyColor, glm::vec3(-30.0f, 45.0f, 0.0f), camera);
        drawCube3D(glm::vec3(0.2f, baseY + 0.001f, 0.202f), glm::vec3(0.04f, 0.02f, 0.01f), glm::vec3(0.78f, 0.42f, 0.45f), glm::vec3(25.0f, 0.0f, 0.0f), camera);
        drawCube3D(glm::vec3(0.2f, baseY + 0.001f, 0.206f), glm::vec3(0.032f, 0.001f, 0.01f), glm::vec3(0.8f, 0.42f, 0.45f), glm::vec3(25.0f, 0.0f, 0.0f), camera);
    }
}

void Renderer3D::drawLogWall(Camera* camera, const glm::vec3& centerPos, float radius) {
    if (!logWall || !modelShader || !camera)
        return;

    glUseProgram(modelShader);
    glUniformMatrix4fv(glGetUniformLocation(modelShader, "uView"), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(modelShader, "uProjection"), 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix()));
    glUniform3fv(glGetUniformLocation(modelShader, "uLightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(modelShader, "uLightColor"), 1, glm::value_ptr(lightColor));
    glUniform3fv(glGetUniformLocation(modelShader, "uViewPos"), 1, glm::value_ptr(camera->position));

    const int wallCount = 20;
    const float angleStep = glm::two_pi<float>() / wallCount;

    for (int i = 0; i < wallCount; i++) {
        float angle = i * angleStep;
        glm::vec3 pos;
        pos.x = centerPos.x + cos(angle) * radius;
        pos.z = centerPos.z + sin(angle) * radius;
        pos.y = centerPos.y;

        glm::mat4 M = glm::mat4(1.0f);
        M = glm::translate(M, pos);
        M = glm::rotate(M, -angle, glm::vec3(0, 1, 0));
        M = glm::scale(M, glm::vec3(0.02f));
        M = glm::translate(M, { pos.x, pos.y + 80.0f, pos.z });

        glUniformMatrix4fv(glGetUniformLocation(modelShader, "uModel"), 1, GL_FALSE, glm::value_ptr(M));
        logWall->Draw(modelShader);
    }
}

void Renderer3D::renderPlatform(Camera* camera, const glm::vec3& position, float rotationAngle) {
    if (!platform || !modelShader || !camera)
        return;

    glUseProgram(modelShader);

    glm::mat4 M = glm::translate(glm::mat4(1.0f), position);
    M = glm::rotate(M, rotationAngle, glm::vec3(0, 1, 0));
    M = glm::scale(M, glm::vec3(0.002f));
    M = glm::translate(M, { -325.0f, 0.0f, 0.0f });

    glUniformMatrix4fv(glGetUniformLocation(modelShader, "uModel"), 1, GL_FALSE, glm::value_ptr(M));
    glUniformMatrix4fv(glGetUniformLocation(modelShader, "uView"), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(modelShader, "uProjection"), 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix()));
    glUniform3fv(glGetUniformLocation(modelShader, "uLightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(modelShader, "uLightColor"), 1, glm::value_ptr(lightColor));
    glUniform3fv(glGetUniformLocation(modelShader, "uViewPos"), 1, glm::value_ptr(camera->position));

    platform->Draw(modelShader);
}

void Renderer3D::drawCylinder3D(Camera* camera, glm::vec3 position, glm::vec3 scale, unsigned int texture) {
    if (!shader3DTexture || !camera)
        return;

    glUseProgram(shader3DTexture);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, scale);

    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();

    glUniformMatrix4fv(glGetUniformLocation(shader3DTexture, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shader3DTexture, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader3DTexture, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));

    glUniform3fv(glGetUniformLocation(shader3DTexture, "uLightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shader3DTexture, "uLightColor"), 1, glm::value_ptr(lightColor));
    glUniform3fv(glGetUniformLocation(shader3DTexture, "uViewPos"), 1, glm::value_ptr(camera->position));

    setTextureTiling(shader3DTexture, 3.0f, 1.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shader3DTexture, "uTexture"), 0);

    glBindVertexArray(cylinderVAO);
    glDrawArrays(GL_TRIANGLES, 0, cylinderVertexCount);
    glBindVertexArray(0);
}
