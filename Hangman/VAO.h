#pragma once
#ifndef VAO_H
#define VAO_H

extern unsigned int rectVAO, rectVBO;
extern unsigned int circVAO, circVBO;
extern unsigned int arcVAO, arcVBO;
extern const int CIRC_SLICES;

void initRectVAO();
void initCircleVAO();
void initArcVAO(int slices = 64);

#endif