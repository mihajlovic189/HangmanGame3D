#include "Primitives3D.h"
#include <GL/glew.h>
#include <cmath>

unsigned int cubeVAO = 0, cubeVBO = 0;
unsigned int sphereVAO = 0, sphereVBO = 0, sphereEBO = 0;
unsigned int sphereIndexCount = 0;
unsigned int cylinderVertexCount = 0;
unsigned int planeVAO = 0, planeVBO = 0;
unsigned int cylinderVAO = 0, cylinderVBO = 0;

std::vector<float> generateCubeVertices() {
    // Format: x, y, z, nx, ny, nz, u, v
    std::vector<float> vertices = {
        // Prednja strana (Z+)
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        // Zadnja strana (Z-)
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,

        // Leva strana (X-)
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,

        // Desna strana (X+)
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,

         // Gornja strana (Y+)
         -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
          0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,

         // Donja strana (Y-)
         -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
          0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
          0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
          0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f
    };

    return vertices;
}

std::vector<float> generateSphereVertices(int latBands, int lonBands, std::vector<unsigned int>& indices) {
    std::vector<float> vertices;
    const float PI = 3.14159265359f;

    // Generisanje verteksa
    for (int lat = 0; lat <= latBands; ++lat) {
        float theta = lat * PI / latBands;
        float sinTheta = std::sin(theta);
        float cosTheta = std::cos(theta);

        for (int lon = 0; lon <= lonBands; ++lon) {
            float phi = lon * 2 * PI / lonBands;
            float sinPhi = std::sin(phi);
            float cosPhi = std::cos(phi);

            float x = cosPhi * sinTheta;
            float y = cosTheta;
            float z = sinPhi * sinTheta;

            // Pozicija
            vertices.push_back(x * 0.5f);
            vertices.push_back(y * 0.5f);
            vertices.push_back(z * 0.5f);

            // Normala 
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Texture koordinate
            float u = (float)lon / (float)lonBands;
            float v = (float)lat / (float)latBands;
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    // Generisanje indeksa za trouglove
    for (int lat = 0; lat < latBands; ++lat) {
        for (int lon = 0; lon < lonBands; ++lon) {
            int first = lat * (lonBands + 1) + lon;
            int second = first + lonBands + 1;

            indices.push_back(first);
            indices.push_back(first + 1);
            indices.push_back(second);

            indices.push_back(second);
            indices.push_back(first + 1);
            indices.push_back(second + 1);
        }
    }

    return vertices;
}

void initCubeVAO() {
    std::vector<float> vertices = generateCubeVertices();

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Stride je sada 8 floats (pozicija + normala + texCoords)
    int stride = 8 * sizeof(float);

    // Pozicija (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // Normala (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture koordinate (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void initSphereVAO(int latitudeBands, int longitudeBands) {
    std::vector<unsigned int> indices;
    std::vector<float> vertices = generateSphereVertices(latitudeBands, longitudeBands, indices);

    sphereIndexCount = static_cast<unsigned int>(indices.size());

    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Stride je sada 8 floats (pozicija + normala + texCoords)
    int stride = 8 * sizeof(float);

    // Pozicija (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // Normala (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture koordinate (location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void initPlaneVAO() {
    float planeVertices[] = {
        // pozicije           // normale          // texture koordinate
        -0.5f,  0.0f, -0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
         0.5f,  0.0f, -0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
         0.5f,  0.0f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,

        -0.5f,  0.0f, -0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
         0.5f,  0.0f,  0.5f,  0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        -0.5f,  0.0f,  0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f
    };

    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);

    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    // Pozicija
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    // Normala
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    // Texture koordinate
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
}

void initCylinderVAO(int segments, float radius, float height)
{
    std::vector<float> vertices;

    float halfH = height * 0.5f;
    const float PI = 3.14159265359f;

    for (int i = 0; i < segments; i++)
    {
        float theta1 = (float)i / segments * 2.0f * PI;
        float theta2 = (float)(i + 1) / segments * 2.0f * PI;

        float x1 = cos(theta1) * radius;
        float z1 = sin(theta1) * radius;

        float x2 = cos(theta2) * radius;
        float z2 = sin(theta2) * radius;

        // NORMALA POKAZUJE NAPOLJE (pozitivna)
        float nx1 = x1 / radius;
        float nz1 = z1 / radius;
        float nx2 = x2 / radius;
        float nz2 = z2 / radius;

        float u1 = (float)i / segments;
        float u2 = (float)(i + 1) / segments;

        // Donji trougao
        vertices.insert(vertices.end(), {
            x1, -halfH, z1,  nx1, 0, nz1,  u1, 0,
            x2, -halfH, z2,  nx2, 0, nz2,  u2, 0,
            x2,  halfH, z2,  nx2, 0, nz2,  u2, 1
            });

        // Gornji trougao
        vertices.insert(vertices.end(), {
            x1, -halfH, z1,  nx1, 0, nz1,  u1, 0,
            x2,  halfH, z2,  nx2, 0, nz2,  u2, 1,
            x1,  halfH, z1,  nx1, 0, nz1,  u1, 1
            });
    }

    glGenVertexArrays(1, &cylinderVAO);
    glGenBuffers(1, &cylinderVBO);

    glBindVertexArray(cylinderVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    cylinderVertexCount = vertices.size() / 8;
}

// Helper funkcija za postavljanje tiling-a
void setTextureTiling(unsigned int shaderProgram, float tilingX, float tilingY) {
    glUseProgram(shaderProgram);
    GLint tilingLoc = glGetUniformLocation(shaderProgram, "uTiling");
    if (tilingLoc != -1) {
        glUniform2f(tilingLoc, tilingX, tilingY);
    }
}
