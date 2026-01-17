#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

extern unsigned int rectVAO, rectVBO;
extern unsigned int circVAO, circVBO;
extern unsigned int arcVAO, arcVBO;
extern unsigned int texVAO, texVBO;

extern unsigned int texProgram;
extern unsigned int studentTex;
extern unsigned int blackboardTex;
extern unsigned int gallowsTex;

void initTextureQuad();
void drawBlackboard();
void drawGallows();
void drawStudentTexture();

#endif
