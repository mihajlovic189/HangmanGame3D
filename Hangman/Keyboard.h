#pragma once
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <vector>
#include <string>
#include <GLFW/glfw3.h>

enum class KeyType {
    Letter,
    Tab,
    Enter,
    Backspace,
    Escape,
    Reset
};

struct KeyButton {
    char letter = 0;
    KeyType type = KeyType::Letter;
    float x, y;
    float w, h;
    bool clicked = false;
};

class KeyboardManager {
public:
    KeyboardManager();
    
    void createKeyboard();
    void drawKeyboard();
    bool handleMouseClick(float ndcX, float ndcY, bool gameFinished, bool& wordMode, 
                         std::string& inputBuffer, const std::string& secret);
    
    const std::vector<KeyButton>& getButtons() const { return buttons; }
    bool isLetterTried(char c) const;
    
    void setTriedLettersCallback(bool (*callback)(char)) { alreadyTriedCallback = callback; }

private:
    std::vector<KeyButton> buttons;
    bool (*alreadyTriedCallback)(char);
};

#endif
