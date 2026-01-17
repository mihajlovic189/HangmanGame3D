#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <thread>
#include <chrono>
#include <cmath>
#include <map>
#include <fstream>
#include "Util.h"
#include <ft2build.h>
#include "Texture.h"  
#include "VAO.h"        
#include FT_FREETYPE_H

void drawRect(float x, float y, float w, float h, float r, float g, float b, float a);
void initFreeType(const char* fontPath, unsigned int fontSize); 
void renderText(const std::string &text, float x, float y, float scale, float r, float g, float b); 
void drawLetter(char c, float x, float y, float cellW, float cellH, float a);
void calculateScore(); 

    struct Character {
        unsigned int TextureID;  
        int SizeX, SizeY;
        int BearingX, BearingY; 
        unsigned int Advance;    
    };

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

    std::vector<KeyButton> keyboardButtons;

    static std::map<char, Character> Characters; 
    static unsigned int textShader = 0;
    static unsigned int textVAO = 0, textVBO = 0;

    struct GameState {
        std::string secret;
        std::string current;
        std::vector<char> tried;
		std::vector<std::string> triedWords;
        int wrong =0;
        int maxWrong =8;
        bool finished = false;
        bool win = false;
    };

    static GameState gs;
    static unsigned int shaderProgram;
    static bool wordMode = false; 
    static std::string inputBuffer; 

    static int g_windowWidth = 1920;
    static int g_windowHeight = 1080;

    static int currentScore = 0;
    static int bestScore = 0;
    static int correctLetters = 0; 
    static int incorrectLetters = 0; 
    static int incorrectWords = 0; 

    static const char* BEST_SCORE_FILE = "bestscore.txt";

    void loadBestScoreFromFile() {
        std::ifstream in(BEST_SCORE_FILE);
        if (!in.is_open()) {
            return;
        }
        int value = 0;
        if (in >> value) {
            bestScore = value;
        }
    }

    void saveBestScoreToFile() {
        std::ofstream out(BEST_SCORE_FILE, std::ios::trunc);
        if (!out.is_open()) {
            std::cout << "[WARN] Cannot open bestscore.txt for writing" << std::endl;
            return;
        }
        out << bestScore;
    }

    const std::vector<std::string> WORDS = {
		"PROGRAM", "RACUNAR", "TASTATURA", "MONITOR", "PETLJA", "FAJL", "KOMPAJLER", "OBJEKAT", "POKAZIVAC", "NIZ", "KLASA", "FUNKCIJA", "MIS", "HARDVER", "SOFTVER", "MEMORIJA", "DISK", "MREZA", "INTERNET", "PRETRAZIVAC"
    };

    void endProgram(const char* msg) {
         std::cout << msg << std::endl;
         glfwTerminate();
         std::exit(-1);
    }

    void drawRect(float x, float y, float w, float h, float r, float g, float b, float a){
        glUseProgram(shaderProgram);
        glUniform4f(glGetUniformLocation(shaderProgram,"uColor"), r,g,b,a);
        glUniform2f(glGetUniformLocation(shaderProgram,"uPos"), x,y);
        glUniform2f(glGetUniformLocation(shaderProgram,"uScale"), w,h);
        glBindVertexArray(rectVAO);
        glDrawArrays(GL_TRIANGLE_FAN,0,4);
    }    

    void drawHead(float x, float y, float w, float h, float r, float g, float b, float a) {
        glUseProgram(shaderProgram);
        glUniform4f(glGetUniformLocation(shaderProgram, "uColor"), r, g, b, a);
        glUniform2f(glGetUniformLocation(shaderProgram, "uPos"), x, y);
        glUniform2f(glGetUniformLocation(shaderProgram, "uScale"), w, h);
        glBindVertexArray(circVAO);
        glLineWidth(8.0f);    
        glDrawArrays(GL_LINE_LOOP, 0, 14);   
    }    

    void drawArc(float x, float y, float w, float h, float startDeg, float endDeg, float r, float g, float b, float a){
        glUseProgram(shaderProgram);
        glUniform4f(glGetUniformLocation(shaderProgram, "uColor"), r, g, b, a);
        glUniform2f(glGetUniformLocation(shaderProgram, "uPos"), x, y);
        glUniform2f(glGetUniformLocation(shaderProgram, "uScale"), w, h);
        glBindVertexArray(arcVAO);
        glLineWidth(5.0f);

        int slices = 64;
        int startIndex = (int)(startDeg / 360.0f * slices) % slices;
        int endIndex = (int)(endDeg / 360.0f * slices) % slices;

        if (endIndex < startIndex)
            endIndex += slices;

        glDrawArrays(GL_LINE_STRIP, startIndex, endIndex - startIndex + 1);
    }

    void drawLetter(char c, float x, float y, float cellW, float cellH, float a)
    {
        float scale = cellH * 10.0f; 

        float r = a, g = a, b = a;

        std::string s(1, c);
        renderText(s, x + 0.02f, y + 0.04f, scale, r, g, b);
    }

    void drawWord() {
        int L = (int)gs.current.size();
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

        for (int i = 0; i < L; ++i)
        {
            float x0 = realStartX + i * cellW;

            drawRect(x0 + cellW / 2.0f, baseY + cellH / 2.0f,
                cellW * 0.98f, cellH * 0.98f,
                0.2f, 0.2f, 0.2f, 0.7f);

            if (gs.current[i] != '_')
                drawLetter(gs.current[i], x0, baseY, cellW * 0.9f, cellH, 1.0f);
        }
    }

    void createKeyboard() {
        keyboardButtons.clear();

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

            keyboardButtons.push_back(kb);
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
            keyboardButtons.push_back(kb);
        }
    }

    void drawInputBuffer() {
        if (!wordMode)
            return;

        int L = (int)gs.secret.size();

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

        for (size_t i = 0; i < inputBuffer.size(); ++i) {
            float x = startX + i * cellW;
            drawRect(x + cellW / 2.0f, y + cellH / 2.0f, cellW * 0.95f, cellH * 0.95f, 0.15f, 0.15f, 0.15f, 1.0f);
            drawLetter(inputBuffer[i], x+0.02f, y-0.02f, cellW * 0.8f, cellH, 1.0f);
        }

        for (size_t slot = inputBuffer.size(); slot < gs.secret.size(); ++slot) {
            float x = startX + slot * cellW;
            drawRect(x + cellW / 2.0f, y + cellH / 2.0f, cellW * 0.95f, cellH * 0.95f, 0.08f, 0.08f, 0.08f, 1.0f);
        }

        if (inputBuffer.size() < gs.secret.size()) {
            float cursorX = startX + inputBuffer.size() * cellW + cellW / 2.0f;
            drawRect(cursorX, y + cellH / 2.0f, cellW * 0.2f, cellH * 0.9f, 0.6f, 0.6f, 0.0f, 1.0f);
        }
    }

    void drawTriedLetters() {

        renderText("ATTEMPTED LETTERS:", -0.95f, 0.61f, 0.7f, 1.0f, 1.0f, 1.0f);

        if(!gs.tried.empty()){
            std::string tried;
            for(char c : gs.tried) {
                tried += c;
                tried += ' ';
            }
            renderText(tried, -0.55f, 0.61f, 0.7f, 1.0f, 1.0f, 0.3f);
        }
    }

    void drawTriedWords() {

        renderText("ATTEMPTED WORDS:", -0.95f, 0.70f, 0.7f, 1.0f, 1.0f, 1.0f);

        if (!gs.triedWords.empty()) {

            std::string tried;
            for (const std::string& w : gs.triedWords) {
                tried += w;
                tried += "  ";  
            }

            renderText(tried, -0.57f, 0.70f, 0.7f, 1.0f, 1.0f, 0.3f);
        }
    }

    void drawInstructions() {
        std::vector<std::string> lines = {
            "TAB - ENTER WORD",
            "ENTER - ACCEPT",
            "BACKSPACE - DELETE",
            "CTRL+R - RESET",
            "ESC - QUIT"
        };

        float x = -0.99f;
        for(const auto& line : lines){
            renderText(line, x, 0.95f, 0.5f, 1.0f, 1.0f, 1.0f);
            x += 0.35f;
        }
    }

    void drawWinLoss(bool res) {
        std::string message = res ? "YOU WON" : "YOU LOST";
        
        float startX = -0.255f;
        float startY = 0.0f;
        float cellH = 0.185f;
        float cellW = 0.215f;
        float spacing = 1.1f;
        
        if(res)
            renderText(message, startX, startY, cellW * 10.0f, 0.0f, 1.0f, 0.0f);
        else
            renderText(message, startX, startY, cellW * 10.0f, 1.0f, 0.0f, 0.0f);
    }

    void drawHangman() {
        float shiftX = -0.43f;       
        float s = 0.5625f;            

        float headX = shiftX + 0.22f * s;
        float headY = 0.25f;

        if (gs.wrong >= 1) 
			drawHead(headX, headY, 0.08f * s, 0.08f, 0.09f, 0.09f, 0.09f, 1.0f); 		 // glava
        if (gs.wrong >= 2) 
			drawRect(headX, headY - 0.385f * s, 0.01f, 0.232f, 0.09f, 0.09f, 0.09f, 1.0f); // tijelo
        if (gs.wrong >= 3) {
            drawRect(headX - 0.05f * s, headY - 0.365f * s, 0.01f, 0.19f, 0.09f, 0.09f, 0.09f, 1.0f);  // lijeva ruka
			drawRect(headX - 0.02f * s, headY - 0.215f * s, 0.03f, 0.018f, 0.09f, 0.09f, 0.09f, 1.0f); // lijevo rame
        }
        if (gs.wrong >= 4) {
			drawRect(headX + 0.05f * s, headY - 0.365f * s, 0.01f, 0.19f, 0.09f, 0.09f, 0.09f, 1.0f);  // desna ruka
			drawRect(headX + 0.02f * s, headY - 0.215f * s, 0.03f, 0.018f, 0.09f, 0.09f, 0.09f, 1.0f); // desno rame
        }
        if (gs.wrong >= 5) {
            drawRect(headX - 0.03f * s, headY - 0.75f * s, 0.01f, 0.19f, 0.09f, 0.09f, 0.09f, 1.0f);   // lijeva noga
			drawRect(headX - 0.02f * s, headY - 0.575f * s, 0.021f, 0.018f, 0.09f, 0.09f, 0.09f, 1.0f);  // lijevi kuk 
        }
        if (gs.wrong >= 6) {
			drawRect(headX + 0.03f * s, headY - 0.75f * s, 0.01f, 0.19f, 0.09f, 0.09f, 0.09f, 1.0f);   // desna noga
            drawRect(headX + 0.02f * s, headY - 0.575f * s, 0.021f, 0.018f, 0.09f, 0.09f, 0.09f, 1.0f);  // desni kuk 
        }
        if (gs.wrong >= 7) {
            drawRect(headX - 0.03f * s, headY + 0.035f * s, 0.02f * s, 0.02f * s, 0.09f, 0.09f, 0.09f, 1.0f);  // oko
			drawRect(headX + 0.03f * s, headY + 0.035f * s, 0.02f * s, 0.02f * s, 0.09f, 0.09f, 0.09f, 1.0f);  // oko
        }
        if (gs.wrong >= 8) {
            drawArc(headX, headY - 0.1f * s, 0.025f, 0.03f, 30, 150, 0.09f, 0.09f, 0.09f, 1.0f);  // usta
        }
    }

    void calculateScore() {
        if(gs.finished && gs.win && currentScore > bestScore) {
            bestScore = currentScore;
            saveBestScoreToFile();
        }
    }

    void drawScore() {
        std::string scoreText = "SCORE: " + std::to_string(currentScore);
        std::string bestText = "BEST: " + std::to_string(bestScore);

        renderText(scoreText, -0.95f, 0.79f, 0.8f, 1.0f, 1.0f, 0.0f);
        renderText(bestText, -0.95f, 0.88f, 0.8f, 0.0f, 1.0f, 0.0f);
    }

    bool alreadyTried(char c){
        return std::find(gs.tried.begin(), gs.tried.end(), c) != gs.tried.end();
    }

    bool alreadyTriedWord(const std::string& w) {
        return std::find(gs.triedWords.begin(), gs.triedWords.end(), w) != gs.triedWords.end();
    }

    void applyGuess(char c){
        if(gs.finished || wordMode) 
            return;
        if(alreadyTried(c)) 
            return;
        gs.tried.push_back(c);
        bool hit=false;
        int lettersFound = 0;
        for(size_t i=0;i<gs.secret.size();++i)
        { 
            if(gs.secret[i]==c)
            { 
                gs.current[i]=c; 
                hit=true;
                lettersFound++;
            } 
        }
        if(!hit) 
        {
            gs.wrong++;
            incorrectLetters++; 
            currentScore -= 80; 
        } 
        else 
        {
            correctLetters += lettersFound;
            currentScore += 100 * lettersFound; 
        }
     
        if(gs.current == gs.secret)
        { 
            gs.finished=true; 
            gs.win=true;   
            calculateScore();
        }
        else if(gs.wrong>=gs.maxWrong)
        { 
            gs.finished=true; 
            gs.win=false; 
        }
    }

    void tryWholeWord(){

        if(inputBuffer.empty()) 
            return;
        if(inputBuffer.size() != gs.secret.size()) 
            return; 
        std::string guess = inputBuffer;

        for(char &ch: guess) 
            ch = (char)toupper(ch);
        if (alreadyTriedWord(guess))
            return;
        gs.triedWords.push_back(guess);
        if(guess == gs.secret)
        {
            int totalLetters = (int)gs.secret.size();
            int unguessedLetters = 0;
            for(char c : gs.current) {
                if(c == '_') 
                    unguessedLetters++;
            }

            gs.current = gs.secret;
            int wordBonus = (int)((1000.0f / totalLetters) * unguessedLetters);
            currentScore += wordBonus;
            
            gs.finished = true; 
            gs.win = true;
        } else {
            gs.wrong++;
            incorrectWords++; 
            currentScore -= 125; 
            if(gs.wrong>=gs.maxWrong)
            { 
                gs.finished=true; 
                gs.win=false; 
            }
        }
        inputBuffer.clear();
        wordMode = false;
    }

    void resetGame(){
        gs.secret = WORDS[std::rand() % WORDS.size()];
        gs.current.assign(gs.secret.size(), '_');
        gs.tried.clear();
        gs.triedWords.clear();
        gs.wrong =0;
        gs.finished = false;
        gs.win = false;
        wordMode = false;
        inputBuffer.clear();
        currentScore = 0;        
        createKeyboard();
    }

    void drawKeyboard() {
        for (auto& k : keyboardButtons) {
            float r = 0.2f, g = 0.25f, b = 0.3f, a = 0.7f;
            std::string label;
            if (k.type == KeyType::Letter) {
                if (alreadyTried(k.letter)) {
                    drawRect(k.x + k.w / 2, k.y + k.h / 2, k.w * 0.6f, k.h * 0.8f, 0.2f, 0.2f, 0.2f, a);
                    drawLetter(k.letter, k.x + 0.015f, k.y - 0.02f, k.w, k.h * 0.9f, 0.5f);
                }
                else {
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

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
        if(action == GLFW_PRESS)
        {
           if(key == GLFW_KEY_ESCAPE)
           { 
               glfwSetWindowShouldClose(window, true); 
           }
           else if(key == GLFW_KEY_R && (mods & GLFW_MOD_CONTROL))
           { 
               resetGame(); 
           }
           else if(key == GLFW_KEY_TAB)
           { 
               if(!gs.finished)
               { 
                   wordMode = !wordMode; 
                   inputBuffer.clear(); 
               } 
           }
           else if(wordMode)
           {
               if(key == GLFW_KEY_ENTER)
               { 
                   tryWholeWord(); 
               }
               else if(key == GLFW_KEY_BACKSPACE)
               { 
                   if(!inputBuffer.empty()) 
                       inputBuffer.pop_back(); 
               }
               else if(key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
               { 
                   if(inputBuffer.size() < gs.secret.size()) 
                       inputBuffer.push_back('A' + (key - GLFW_KEY_A)); 
               }
           }
           else if(key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
           { 
               char c = 'A' + (key - GLFW_KEY_A); 
               applyGuess(c); 
           }
        }
    }

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

            double mx, my;
            glfwGetCursorPos(window, &mx, &my);

            // pretvori u NDC koordinate
            float nx = (mx / g_windowWidth) * 2.0f - 1.0f;
            float ny = 1.0f - (my / g_windowHeight) * 2.0f;

            for (auto& k : keyboardButtons) {
                float x1 = k.x;
                float y1 = k.y;
                float x2 = k.x + k.w;
                float y2 = k.y + k.h;

                if (nx >= x1 && nx <= x2 && ny >= y1 && ny <= y2) {
                    if (!gs.finished) {
                        switch (k.type) {
                        case KeyType::Letter:
                            if (!alreadyTried(k.letter) && !wordMode)
                                applyGuess(k.letter);
                            else if (wordMode && inputBuffer.size() < gs.secret.size())
                                inputBuffer.push_back(k.letter);
                            break;
                        case KeyType::Tab:
                            wordMode = !wordMode;
                            inputBuffer.clear();
                            break;
                        case KeyType::Enter:
                            if (wordMode)
                                tryWholeWord();
                            break;
                        case KeyType::Backspace:
                            if (wordMode && !inputBuffer.empty())
                                inputBuffer.pop_back();
                            break;
                        case KeyType::Reset:
                            resetGame();
                            break;
                        default:
                            break;
                        }
                    }
                    else {
                        switch (k.type) {
                            case KeyType::Reset:
                                resetGame();
                                break;  
                            default:
                                break;
                        }
                    }
                }
            }
        }
    }

    void drawResult(){
        if(gs.finished)
        {
			drawWinLoss(gs.win);
        }
    }

    // Helper funkcija: konverzija NDC (-1 do 1) u screen space (0 do širina/visina)
    void ndcToScreen(float ndcX, float ndcY, float &screenX, float &screenY, int width, int height) {
        screenX = (ndcX + 1.0f) * 0.5f * width;
        screenY = (1.0f - ndcY) * 0.5f * height; // flip Y jer je NDC inverzno
    }

    void initFreeType(const char* fontPath, unsigned int fontSize){
    
        FT_Library ft;
        if(FT_Init_FreeType(&ft)){
            return;
        }

        FT_Face face;
    
        const char* fontPaths[] = {
            "x64/Debug/arial.ttf",    
            "../arial.ttf",     
            "C:/Windows/Fonts/arial.ttf",     
            "C:\\Windows\\Fonts\\arial.ttf"    
        };
    
        bool fontLoaded = false;
        for(int i = 0; i < 6; i++){
            if(FT_New_Face(ft, fontPaths[i], 0, &face) == 0){
                fontLoaded = true;
                break;
            }
        }
  
        if(!fontLoaded){
            FT_Done_FreeType(ft);
            return;
        }

        FT_Set_Pixel_Sizes(face, 0, fontSize);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for(unsigned char c = 0; c < 128; c++){
            if(FT_Load_Char(face, c, FT_LOAD_RENDER)){
                continue;
            }

            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RED,
            face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            Character character = { texture, (int)face->glyph->bitmap.width, (int)face->glyph->bitmap.rows, face->glyph->bitmap_left, face->glyph->bitmap_top, (unsigned int)face->glyph->advance.x };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);

        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        glGenVertexArrays(1, &textVAO);
        glGenBuffers(1, &textVBO);
        glBindVertexArray(textVAO);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void renderText(const std::string &text, float ndcX, float ndcY, float scale, float r, float g, float b){
        if(!textShader) {
            return;
        }
        if(Characters.empty()) {
            return;
        }
    
        float screenX, screenY;
        ndcToScreen(ndcX, ndcY, screenX, screenY, g_windowWidth, g_windowHeight);
    
        glUseProgram(textShader);
        glUniform3f(glGetUniformLocation(textShader, "textColor"), r, g, b);
        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(textVAO);

        float x = screenX;
        float y = screenY;

        for(char c : text){
            if(Characters.find(c) == Characters.end()) {
                continue; 
            }

            Character ch = Characters[c];

            float xpos = x + ch.BearingX * scale;
            float ypos = y + (ch.SizeY - ch.BearingY) * scale; 

            float w = ch.SizeX * scale;
            float h = ch.SizeY * scale;

            
            float vertices[6][4] = {
                { xpos, ypos - h, 0.0f, 0.0f },  
                { xpos, ypos, 0.0f, 1.0f },
                { xpos + w, ypos, 1.0f, 1.0f },

                { xpos,ypos - h, 0.0f, 0.0f },  
                { xpos + w, ypos, 1.0f, 1.0f },
                { xpos + w, ypos - h, 1.0f, 0.0f }   
            };

            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            x += (ch.Advance >> 6) * scale; // Deli sa 64 (bit shift = efikasnije od /64)
        }
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    int main(){
        std::srand((unsigned)std::time(nullptr));
        gs.secret = WORDS[std::rand() % WORDS.size()];
        gs.current.assign(gs.secret.size(), '_');

        if(!glfwInit()) 
            endProgram("GLFW init fail");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        GLFWmonitor* mon = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(mon);
        GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Hangman", mon, nullptr);
        if(!window)
            return -1; 

        glfwMakeContextCurrent(window);

        glfwGetFramebufferSize(window, &g_windowWidth, &g_windowHeight);

        glfwSetKeyCallback(window, key_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glewExperimental = GL_TRUE;
        if(glewInit()!=GLEW_OK)
        { 
            return -1; 
        }
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        shaderProgram = createShader("color.vert","color.frag");
        if(!shaderProgram) {
            glfwTerminate();
            return -1;
        }

        texProgram = createShader("tex.vert","tex.frag");
        if(!texProgram) {
            glfwTerminate();
            return -1;
        }
    
        initRectVAO();
        initTextureQuad();
        initCircleVAO();
        initArcVAO();

        textShader = createShader("text.vert", "text.frag");
 
        glUseProgram(textShader);
        float projection[16] = {
            2.0f/g_windowWidth, 0.0f, 0.0f, 0.0f,
            0.0f, -2.0f/g_windowHeight, 0.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f, 1.0f
        };
        glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, projection);
    
        initFreeType("arial.ttf", 48);
        if(Characters.empty()){
            textShader = 0; 
        }
   
        // ISCRTAVANJE TEKSTURA
        studentTex = loadImageToTexture("student.png");
        if(!studentTex){ 
            std::cout << "[WARN] student.png not loaded" << std::endl; 
        } 

        blackboardTex = loadImageToTexture("blackboard.jpg");
        if(!blackboardTex){ 
            std::cout << "[WARN] blackboard.jpg not loaded" << std::endl; 
        } 

        gallowsTex = loadImageToTexture("gallows3.png");
        if (!gallowsTex) {
            std::cout << "[WARN] gallows.png not loaded" << std::endl;
        }

        GLFWcursor* customCursor = loadImageToCursor("cursor.png"); 
        if(customCursor){
            glfwSetCursor(window, customCursor);
        } 

        // POZADINA
        glClearColor(0.2f,0.25f,0.3f,1.0f);
        
        loadBestScoreFromFile();
 
        createKeyboard();

        const double targetFPS =75.0;
        const double frameTime =1.0 / targetFPS; 
        double lastTime = glfwGetTime();

        int frameCount = 0;

        while (!glfwWindowShouldClose(window)) {
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(window, true);
            }

            double now = glfwGetTime();
            double dt = now - lastTime;
            if (dt < frameTime)
            {
                double sleepMs = (frameTime - dt) * 1000.0;
                if (sleepMs > 0)
                    std::this_thread::sleep_for(std::chrono::milliseconds((int)sleepMs));

                now = glfwGetTime();
                dt = now - lastTime;
                if (dt < frameTime)
                    continue;
            }
            lastTime = now;

            glfwPollEvents();  

            calculateScore();
            
            glClear(GL_COLOR_BUFFER_BIT);

            drawBlackboard();
            drawGallows();
            drawHangman();
            drawWord();
            drawInputBuffer();
            drawTriedLetters();
            drawTriedWords();
            drawInstructions();
            drawStudentTexture();
            drawKeyboard();
            drawScore(); 

            drawResult();
            glfwSwapBuffers(window);

            frameCount++;
        }

        glfwTerminate();
        return 0;
    }
    
    static void getWindowSize(GLFWwindow* window, int& width, int& height)
    {
        glfwGetFramebufferSize(window, &width, &height);
    }