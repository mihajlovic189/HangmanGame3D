#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "VAO.h"

unsigned int rectVAO, rectVBO, circVAO = 0, circVBO = 0, arcVAO, arcVBO;
const int CIRC_SLICES = 14;

void initRectVAO() {
    float verts[] = { -0.5f,-0.5f, -0.5f,0.5f,0.5f,0.5f,0.5f,-0.5f }; // Pravougaonik
    glGenVertexArrays(1, &rectVAO); // Kreira 1 VAO, ID čuva u rectVAO
    glGenBuffers(1, &rectVBO); // Kreira buffer
    glBindVertexArray(rectVAO); // "Aktivira" ovaj VAO za konfigurisanje
    glBindBuffer(GL_ARRAY_BUFFER, rectVBO); // Aktivira bufer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // Šalje podatke u GPU memoriju
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0); // Objašnjava GPU-u kako da čita podatke iz VBO-a
	glEnableVertexAttribArray(0);  // Uključi location 0 (pozicija)
}

void initCircleVAO() {
    std::vector<float> verts;
    verts.reserve(CIRC_SLICES * 2);

    for (int i = 0; i < CIRC_SLICES; i++) {
        float ang = (float)i * 2.0f * 3.1415926535f / (float)CIRC_SLICES;
        verts.push_back(std::cos(ang));
        verts.push_back(std::sin(ang));
    }

    glGenVertexArrays(1, &circVAO);
    glGenBuffers(1, &circVBO);
    glBindVertexArray(circVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circVBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void initArcVAO(int slices) {
    std::vector<float> verts;
    verts.reserve(slices * 2);

    for (int i = 0; i < slices; i++) {
        float ang = (float)i * 2.0f * 3.1415926535f / (float)slices;
        verts.push_back(std::cos(ang));
        verts.push_back(std::sin(ang));
    }

    glGenVertexArrays(1, &arcVAO);
    glGenBuffers(1, &arcVBO);
    glBindVertexArray(arcVAO);

    glBindBuffer(GL_ARRAY_BUFFER, arcVBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

