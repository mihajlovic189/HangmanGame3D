#pragma once
#ifndef PRIMITIVES3D_H
#define PRIMITIVES3D_H

#include <vector>

// VAO i VBO za 3D primitivne oblike
extern unsigned int cubeVAO, cubeVBO;
extern unsigned int sphereVAO, sphereVBO, sphereEBO;
extern unsigned int sphereIndexCount, cylinderVertexCount;
extern unsigned int planeVAO, planeVBO;
extern unsigned int cylinderVAO, cylinderVBO;

// Inicijalizacija 3D primitiva
void initCubeVAO();
void initPlaneVAO();
void initSphereVAO(int latitudeBands = 30, int longitudeBands = 30);
void initCylinderVAO(int segments, float radius, float height);

// Helper funkcije za generisanje geometrije
std::vector<float> generateCubeVertices();
std::vector<float> generateSphereVertices(int latBands, int lonBands, std::vector<unsigned int>& indices);

// Helper funkcija za postavljanje tiling-a u shader-u
void setTextureTiling(unsigned int shaderProgram, float tilingX = 1.0f, float tilingY = 1.0f);

#endif
