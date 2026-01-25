#include "Keyboard.h"
#include <algorithm>

extern void drawRect(float x, float y, float w, float h, float r, float g, float b, float a);
extern void drawLetter(char c, float x, float y, float cellW, float cellH, float a);
extern void renderText(const std::string &text, float x, float y, float scale, float r, float g, float b);

KeyboardManager::KeyboardManager() : alreadyTriedCallback(nullptr) {
    createKeyboard();
}

void KeyboardManager::createKeyboard() {
    buttons.clear();

    float startX = 0.45f;
    float startY = 0.2f;
    float keyW = 0.10f;
    float keyH = 0.10f;
    float spacing = 0.07f;

    for (int i = 0; i < 26; i++) {
        KeyButton kb;
        kb.letter = 'A' + i;
        kb.type = KeyType::Letter;
        kb.x = startX + (i % 7) * spacing;
        kb.y = startY - (i / 7) * 0.15f;
        kb.w = keyW;
        kb.h = keyH;
        buttons.push_back(kb);
    }

    std::vector<std::pair<KeyType, std::string>> specials = {
        {KeyType::Tab, "TAB"},
        {KeyType::Enter, "ENTER"},
        {KeyType::Backspace, "DEL"},
        {KeyType::Reset, "RESET"}
    };

    float specY = startY - 4 * 0.15f;
    for (size_t i = 0; i < specials.size(); ++i) {
        KeyButton kb;
        kb.type = specials[i].first;
        kb.letter = 0;
        kb.x = startX + i * (keyW + 0.02f);
        kb.y = specY;
        kb.w = keyW * 1.2f;
        kb.h = keyH;
        buttons.push_back(kb);
    }
}

void KeyboardManager::drawKeyboard() {
    for (auto& k : buttons) {
        float r = 0.2f, g = 0.25f, b = 0.3f, a = 0.9f;
        std::string label;
        
        if (k.type == KeyType::Letter) {
            bool tried = alreadyTriedCallback ? alreadyTriedCallback(k.letter) : false;
            if (tried) {
                drawRect(k.x + k.w / 2, k.y + k.h / 2, k.w * 0.6f, k.h * 0.8f, 0.2f, 0.2f, 0.2f, a);
                drawLetter(k.letter, k.x + 0.015f, k.y - 0.02f, k.w, k.h * 0.9f, 0.5f);
            } else {
                drawRect(k.x + k.w / 2, k.y + k.h / 2, k.w * 0.6f, k.h * 0.8f, r, g, b, a);
                drawLetter(k.letter, k.x + 0.015f, k.y - 0.02f, k.w, k.h * 0.9f, 0.9f);
            }
        }
        else if (k.type == KeyType::Tab) {
            label = "TAB";
            drawRect(k.x + k.w / 2, k.y + k.h / 2, k.w * 0.8f, k.h * 0.8f, r, g, b, a);
            renderText(label, k.x + 0.027f, k.y + 0.03f, k.w * 5.6f, 1.0f, 1.0f, 1.0f);
        }
        else if (k.type == KeyType::Enter) {
            label = "ENTER";
            drawRect(k.x + k.w / 2, k.y + k.h / 2, k.w, k.h * 0.8f, r, g, b, a);
            renderText(label, k.x + 0.003f, k.y + 0.03f, k.w * 5.6f, 1.0f, 1.0f, 1.0f);
        }
        else if (k.type == KeyType::Backspace) {
            label = "DEL";
            drawRect(k.x + k.w / 2, k.y + k.h / 2, k.w * 0.8f, k.h * 0.8f, r, g, b, a);
            renderText(label, k.x + 0.027f, k.y + 0.03f, k.w * 5.6f, 1.0f, 1.0f, 1.0f);
        }
        else if (k.type == KeyType::Reset) {
            label = "RESET";
            drawRect(k.x + k.w / 2, k.y + k.h / 2, k.w, k.h * 0.8f, r, g, b, 1.0f);
            renderText(label, k.x + 0.003f, k.y + 0.03f, k.w * 5.6f, 1.0f, 1.0f, 1.0f);
        }
    }
}

bool KeyboardManager::handleMouseClick(float ndcX, float ndcY, bool gameFinished, 
                                       bool& wordMode, std::string& inputBuffer, 
                                       const std::string& secret) {
    for (auto& k : buttons) {
        float x1 = k.x;
        float y1 = k.y;
        float x2 = k.x + k.w;
        float y2 = k.y + k.h;

        if (ndcX >= x1 && ndcX <= x2 && ndcY >= y1 && ndcY <= y2) {
            if (!gameFinished) {
                switch (k.type) {
                case KeyType::Letter:
                    if (wordMode && inputBuffer.size() < secret.size())
                        inputBuffer.push_back(k.letter);
                    return true;
                case KeyType::Tab:
                    wordMode = !wordMode;
                    inputBuffer.clear();
                    return true;
                case KeyType::Enter:
                    return true;
                case KeyType::Backspace:
                    if (wordMode && !inputBuffer.empty())
                        inputBuffer.pop_back();
                    return true;
                case KeyType::Reset:
                    return true;
                default:
                    break;
                }
            } else {
                if (k.type == KeyType::Reset)
                    return true;
            }
        }
    }
    return false;
}
