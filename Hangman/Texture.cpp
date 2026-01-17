#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

unsigned int texVAO = 0, texVBO = 0;
unsigned int texProgram = 0;
unsigned int studentTex = 0;
unsigned int blackboardTex = 0;
unsigned int gallowsTex = 0;

void initTextureQuad() {
    float verts[] = {
        -0.5f,-0.5f,0.0f,0.0f, // Donji levi ugao
        -0.5f,0.5f,0.0f,1.0f,  // Gornji levi
        0.5f,0.5f,1.0f,1.0f,   // Gornji desni
        0.5f,-0.5f,1.0f,0.0f   // Donji desni
    };
    glGenVertexArrays(1, &texVAO);
    glGenBuffers(1, &texVBO);
    glBindVertexArray(texVAO);
    glBindBuffer(GL_ARRAY_BUFFER, texVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);  // Uključi location 1 (texture koordinate)
}

void drawStudentTexture() {
    if (!texProgram || !studentTex)
        return;
    glUseProgram(texProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, studentTex);
    glUniform1i(glGetUniformLocation(texProgram, "uTex"), 0);
    glUniform1f(glGetUniformLocation(texProgram, "uAlpha"), 0.3f);
    glUniform2f(glGetUniformLocation(texProgram, "uPos"), 0.80f, 0.9f);
    glUniform2f(glGetUniformLocation(texProgram, "uScale"), 0.4f, 0.2f);
    glBindVertexArray(texVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawBlackboard() {
    if (!texProgram || !blackboardTex)
        return;
    glUseProgram(texProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blackboardTex);
    glUniform1i(glGetUniformLocation(texProgram, "uTex"), 0);
    glUniform1f(glGetUniformLocation(texProgram, "uAlpha"), 1.0f);
    glUniform2f(glGetUniformLocation(texProgram, "uPos"), 0.0f, 0.0f);
    glUniform2f(glGetUniformLocation(texProgram, "uScale"), 2.1f, 2.1f);
    glBindVertexArray(texVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawGallows() {
    if (!texProgram || !gallowsTex)
        return;
    glUseProgram(texProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gallowsTex);
    glUniform1i(glGetUniformLocation(texProgram, "uTex"), 0);
    glUniform1f(glGetUniformLocation(texProgram, "uAlpha"), 0.8f);
    glUniform2f(glGetUniformLocation(texProgram, "uPos"), -0.55f, -0.12f);
    glUniform2f(glGetUniformLocation(texProgram, "uScale"), 1.2f, 1.75f);
    glBindVertexArray(texVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
}