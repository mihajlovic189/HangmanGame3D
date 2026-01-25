#include "UIRenderer.h"
#include "TextRenderer.h"
#include <algorithm>
#include <GL/glew.h>

extern void drawRect(float x, float y, float w, float h, float r, float g, float b, float a);
extern void drawLetter(char c, float x, float y, float cellW, float cellH, float a);
extern void renderText(const std::string& text, float x, float y, float scale, float r, float g, float b);

UIRenderer::UIRenderer() 
    : screenWidth(1920), screenHeight(1080) {
}

void UIRenderer::initialize(int screenWidth, int screenHeight) {
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
}

void UIRenderer::updateScreenSize(int width, int height) {
    this->screenWidth = width;
    this->screenHeight = height;
}

void UIRenderer::drawWord(const GameState& state) {
    int L = (int)state.current.size();
    float startX = -0.8f;
    float baseY = -0.8f;
    float maxWidth = 0.12f;
    float minWidth = 0.03f;
    float cellW = 1.6f / std::max(L, 1);
    float cellH = 0.25f;

    if (cellW > maxWidth)
        cellW = maxWidth;

    if (cellW < minWidth)
        cellW = minWidth;

    float totalWidth = cellW * L;
    float realStartX = -totalWidth / 2.0f;

    for (int i = 0; i < L; ++i) {
        float x0 = realStartX + i * cellW;
        drawRect(x0 + cellW / 2.0f, baseY + cellH / 2.0f, cellW * 0.98f, cellH * 0.98f, 0.2f, 0.2f, 0.2f, 0.9f);

        if (state.current[i] != '_')
            drawLetter(state.current[i], x0, baseY, cellW * 0.9f, cellH, 1.0f);
    }
}

void UIRenderer::drawInputBuffer(const std::string& buffer, const std::string& secret, bool wordMode) {
    if (!wordMode)
        return;

    int L = (int)secret.size();
    float maxWidth = 0.12f;
    float minWidth = 0.03f;
    float cellW = 1.6f / std::max(L, 1);

    if (cellW > maxWidth)
        cellW = maxWidth;

    if (cellW < minWidth)
        cellW = minWidth;

    float cellH = 0.10f;
    float totalWidth = cellW * L;
    float startX = -totalWidth / 2.0f;
    float wordBaseY = -0.8f;
    float y = wordBaseY + 0.30f;

    for (size_t i = 0; i < buffer.size(); ++i) {
        float x = startX + i * cellW;
        drawRect(x + cellW / 2.0f, y + cellH / 2.0f, cellW * 0.95f, cellH * 0.95f, 0.15f, 0.15f, 0.15f, 1.0f);
        drawLetter(buffer[i], x + 0.02f, y - 0.02f, cellW * 0.8f, cellH, 1.0f);
    }

    for (size_t slot = buffer.size(); slot < secret.size(); ++slot) {
        float x = startX + slot * cellW;
        drawRect(x + cellW / 2.0f, y + cellH / 2.0f, cellW * 0.95f, cellH * 0.95f, 0.08f, 0.08f, 0.08f, 1.0f);
    }

    if (buffer.size() < secret.size()) {
        float cursorX = startX + buffer.size() * cellW + cellW / 2.0f;
        drawRect(cursorX, y + cellH / 2.0f, cellW * 0.2f, cellH * 0.9f, 0.6f, 0.6f, 0.0f, 1.0f);
    }
}

void UIRenderer::drawTriedLetters(const std::vector<char>& tried) {
    renderText("LETTERS:", -0.975f, 0.85f, 0.7f, 1.0f, 1.0f, 1.0f);

    if (!tried.empty()) {
        std::string triedStr;
        for (char c : tried) {
            triedStr += c;
            triedStr += ' ';
        }
        renderText(triedStr, -0.80f, 0.85f, 0.7f, 1.0f, 1.0f, 0.3f);
    }
}

void UIRenderer::drawTriedWords(const std::vector<std::string>& triedWords) {
    renderText("WORDS:", -0.975f, 0.76f, 0.7f, 1.0f, 1.0f, 1.0f);

    if (!triedWords.empty()) {
        std::string triedStr;
        for (const std::string& w : triedWords) {
            triedStr += w;
            triedStr += "  ";
        }
        renderText(triedStr, -0.82f, 0.76f, 0.7f, 1.0f, 1.0f, 0.3f);
    }
}

void UIRenderer::drawInstructions() {
    renderText("TAB - WORD MODE |", -0.99f, 0.95f, 0.45f, 1.0f, 1.0f, 1.0f);
    renderText("ENTER - ACCEPT |", -0.76f, 0.95f, 0.45f, 1.0f, 1.0f, 1.0f);
    renderText("BACKSPACE - DELETE |", -0.55, 0.95f, 0.45f, 1.0f, 1.0f, 1.0f);
    renderText("CTRL+R - RESET |", -0.285, 0.95f, 0.45f, 1.0f, 1.0f, 1.0f);
    renderText("CTRL+F - FACE CULL |", -0.08f, 0.95f, 0.45f, 1.0f, 1.0f, 1.0f);
    renderText("ESC - QUIT", 0.25f, 0.95f, 0.45f, 1.0f, 1.0f, 1.0f);
}

void UIRenderer::drawScore(int currentScore, int bestScore) {
    std::string scoreText = "SCORE: " + std::to_string(currentScore);
    std::string bestText = "BEST: " + std::to_string(bestScore);

    renderText(scoreText, -0.95f, -0.79f, 0.8f, 1.0f, 1.0f, 1.0f);
    renderText(bestText, -0.95f, -0.88f, 0.8f, 1.0f, 1.0f, 1.0f);
}

void UIRenderer::drawResult(bool finished, bool win) {
    if (!finished)
        return;

    drawRect(0.0f, 0.0f, 2.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.5f);

    std::string message = win ? "YOU WON" : "YOU LOST";
    
    float startX = -0.855f;
    float startY = 0.0f;
    float textScale = 2.5f;

    if (win) {
        renderText(message, startX, startY, textScale, 0.0f, 1.0f, 0.0f);
    } else {
        renderText(message, startX, startY, textScale, 1.0f, 0.0f, 0.0f);
    }
}
