#pragma once
#ifndef UIRENDERER_H
#define UIRENDERER_H

#include <string>
#include <vector>
#include "GameState.h"

class UIRenderer {
public:
    UIRenderer();
    
    void initialize(int screenWidth, int screenHeight);
    void updateScreenSize(int width, int height);
    
    void drawWord(const GameState& state);
    void drawInputBuffer(const std::string& buffer, const std::string& secret, bool wordMode);
    void drawTriedLetters(const std::vector<char>& tried);
    void drawTriedWords(const std::vector<std::string>& triedWords);
    void drawInstructions();
    void drawScore(int currentScore, int bestScore);
    void drawResult(bool finished, bool win);
    
private:
    
    int screenWidth;
    int screenHeight;
};

#endif
