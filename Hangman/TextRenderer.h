#pragma once
#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <map>
#include <string>

struct Character {
    unsigned int TextureID;
    int SizeX, SizeY;
    int BearingX, BearingY;
    unsigned int Advance;
};

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();
    
    bool initialize(const char* fontPath, unsigned int fontSize, int windowWidth, int windowHeight);
    void renderText(const std::string &text, float ndcX, float ndcY, float scale, float r, float g, float b);
    void drawLetter(char c, float x, float y, float cellW, float cellH, float a);
    
    void updateWindowSize(int width, int height);
    bool isInitialized() const { return initialized; }

private:
    void ndcToScreen(float ndcX, float ndcY, float &screenX, float &screenY, int width, int height);
    
    std::map<char, Character> characters;
    unsigned int textShader;
    unsigned int textVAO, textVBO;
    int windowWidth, windowHeight;
    bool initialized;
};

#endif
