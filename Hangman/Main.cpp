#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <thread>
#include <chrono>
#include "Util.h"
#include "Texture.h"  
#include "VAO.h"
#include "Primitives3D.h"
#include "Camera.h"
#include "model.hpp"
#include "GameState.h"
#include "Keyboard.h"
#include "TextRenderer.h"
#include "UIRenderer.h"
#include "Renderer3D.h"

void drawRect(float x, float y, float w, float h, float r, float g, float b, float a);
void drawLetter(char c, float x, float y, float cellW, float cellH, float a);
void renderText(const std::string &text, float x, float y, float scale, float r, float g, float b);

static GameManager* gameManager = nullptr;
static KeyboardManager* keyboardManager = nullptr;
static TextRenderer* textRenderer = nullptr;
static UIRenderer* uiRenderer = nullptr;
static Renderer3D* renderer3D = nullptr;
static Camera* camera = nullptr;

static unsigned int shaderProgram = 0;
static unsigned int shader3D = 0;
static unsigned int shader3D_with_texture = 0;
static unsigned int modelShader = 0;

static unsigned int groundTexture = 0;
static unsigned int plankTexture = 0;
static unsigned int plankVertTexture = 0;
static unsigned int landscapeTexture = 0;
static unsigned int cobblestoneTexture = 0;
static unsigned int ropeTexture = 0;

Model* logWall = nullptr;
Model* platform = nullptr;

static int g_windowWidth = 1920;
static int g_windowHeight = 1080;
static bool wordMode = false;
static std::string inputBuffer;

static bool faceCullingEnabled = false;

void endProgram(const char* msg) {
    std::cout << msg << std::endl;
    glfwTerminate();
    std::exit(-1);
}

void drawRect(float x, float y, float w, float h, float r, float g, float b, float a) {
    glUseProgram(shaderProgram);
    glUniform4f(glGetUniformLocation(shaderProgram, "uColor"), r, g, b, a);
    glUniform2f(glGetUniformLocation(shaderProgram, "uPos"), x, y);
    glUniform2f(glGetUniformLocation(shaderProgram, "uScale"), w, h);
    glBindVertexArray(rectVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void drawLetter(char c, float x, float y, float cellW, float cellH, float a) {
    if (textRenderer)
        textRenderer->drawLetter(c, x, y, cellW, cellH, a);
}

void renderText(const std::string &text, float x, float y, float scale, float r, float g, float b) {
    if (textRenderer)
        textRenderer->renderText(text, x, y, scale, r, g, b);
}

bool alreadyTriedCallback(char c) {
    return gameManager ? gameManager->alreadyTried(c) : false;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, true);
        }
        else if (key == GLFW_KEY_R && (mods & GLFW_MOD_CONTROL)) {
            gameManager->resetGame();
            keyboardManager->createKeyboard();
            wordMode = false;
            inputBuffer.clear();
        }
        else if (key == GLFW_KEY_F && (mods & GLFW_MOD_CONTROL)) {
            faceCullingEnabled = !faceCullingEnabled;
            if (faceCullingEnabled) {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
                std::cout << "Face Culling: ENABLED" << std::endl;
            } else {
                glDisable(GL_CULL_FACE);
                std::cout << "Face Culling: DISABLED" << std::endl;
            }
        }
        else if (key == GLFW_KEY_TAB) {
            if (!gameManager->getState().finished) {
                wordMode = !wordMode;
                inputBuffer.clear();
            }
        }
        else if (wordMode) {
            if (key == GLFW_KEY_ENTER) {
                if (!inputBuffer.empty()) {
                    std::string guess = inputBuffer;
                    for (char &ch : guess)
                        ch = (char)toupper(ch);
                    gameManager->tryWholeWord(guess);
                    inputBuffer.clear();
                    wordMode = false;
                }
            }
            else if (key == GLFW_KEY_BACKSPACE) {
                if (!inputBuffer.empty())
                    inputBuffer.pop_back();
            }
            else if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
                if (inputBuffer.size() < gameManager->getState().secret.size())
                    inputBuffer.push_back('A' + (key - GLFW_KEY_A));
            }
        }
        else if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z) {
            char c = 'A' + (key - GLFW_KEY_A);
            gameManager->applyGuess(c);
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double mx, my;
        glfwGetCursorPos(window, &mx, &my);

        float nx = (mx / g_windowWidth) * 2.0f - 1.0f;
        float ny = 1.0f - (my / g_windowHeight) * 2.0f;

        for (auto& k : keyboardManager->getButtons()) {
            float x1 = k.x;
            float y1 = k.y;
            float x2 = k.x + k.w;
            float y2 = k.y + k.h;

            if (nx >= x1 && nx <= x2 && ny >= y1 && ny <= y2) {
                if (!gameManager->getState().finished) {
                    switch (k.type) {
                    case KeyType::Letter:
                        if (!gameManager->alreadyTried(k.letter) && !wordMode)
                            gameManager->applyGuess(k.letter);
                        else if (wordMode && inputBuffer.size() < gameManager->getState().secret.size())
                            inputBuffer.push_back(k.letter);
                        break;
                    case KeyType::Tab:
                        wordMode = !wordMode;
                        inputBuffer.clear();
                        break;
                    case KeyType::Enter:
                        if (wordMode && !inputBuffer.empty()) {
                            std::string guess = inputBuffer;
                            for (char &ch : guess)
                                ch = (char)toupper(ch);
                            gameManager->tryWholeWord(guess);
                            inputBuffer.clear();
                            wordMode = false;
                        }
                        break;
                    case KeyType::Backspace:
                        if (wordMode && !inputBuffer.empty())
                            inputBuffer.pop_back();
                        break;
                    case KeyType::Reset:
                        gameManager->resetGame();
                        keyboardManager->createKeyboard();
                        wordMode = false;
                        inputBuffer.clear();
                        break;
                    default:
                        break;
                    }
                }
                else {
                    if (k.type == KeyType::Reset) {
                        gameManager->resetGame();
                        keyboardManager->createKeyboard();
                        wordMode = false;
                        inputBuffer.clear();
                    }
                }
            }
        }
    }
}

int main() {
    std::srand((unsigned)std::time(nullptr));

    if (!glfwInit())
        endProgram("GLFW init fail");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWmonitor* mon = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(mon);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Hangman", mon, nullptr);
    if (!window) {
        endProgram("GLFW window creation failed");
    }

    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &g_windowWidth, &g_windowHeight);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        endProgram("GLEW init failed");
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Inicijalizacija shadera
    shaderProgram = createShader("color.vert", "color.frag");
    texProgram = createShader("tex.vert", "tex.frag");
    shader3D = createShader("shader3d.vert", "shader3d.frag");
    shader3D_with_texture = createShader("shader3d_with_texture.vert", "shader3d_with_texture.frag");
    modelShader = createShader("model.vert", "model.frag");

    if (!shaderProgram || !texProgram || !shader3D || !shader3D_with_texture || !modelShader) {
        glfwTerminate();
        return -1;
    }

	// Inicijalizacija menadzera za igru i ostalih komponenti
    gameManager = new GameManager();
    keyboardManager = new KeyboardManager();
    keyboardManager->setTriedLettersCallback(alreadyTriedCallback);
    
    textRenderer = new TextRenderer();
    textRenderer->initialize("arial.ttf", 48, g_windowWidth, g_windowHeight);
    
    uiRenderer = new UIRenderer();
    uiRenderer->initialize(g_windowWidth, g_windowHeight);
    
    renderer3D = new Renderer3D();

    // Ucitavanje tekstura
    studentTex = loadImageToTextureWithTiling("student.png", false);
    groundTexture = loadImageToTextureWithTiling("grass4.png", true);
    plankTexture = loadImageToTextureWithTiling("res/Mat_1_Base_color.png", true);
    plankVertTexture = loadImageToTextureWithTiling("res/Mat_1_Base_color_vert.png", true);
    cobblestoneTexture = loadImageToTextureWithTiling("cobblestone.jpg", true);
    landscapeTexture = loadImageToTextureWithTiling("landscape.jpg", true);
    ropeTexture = loadImageToTextureWithTiling("rope.jpg", true);

    // Kursor
    GLFWcursor* customCursor = loadImageToCursor("cursor.png");
    if (customCursor) {
        glfwSetCursor(window, customCursor);
    }

    // Inicijalizacija VAO-a
    initRectVAO();
    initTextureQuad();
    initCircleVAO();
    initArcVAO();
    initCubeVAO();
    initSphereVAO();
    initPlaneVAO();
    initCylinderVAO(64, 1.0f, 2.0f);

    // Modeli platforme i zida
    logWall = new Model("res/LogWall.obj");
    platform = new Model("res/Weighted Wood Platform.obj");

    // Inicijalizacija 3D renderer-a
    renderer3D->initialize(shader3D, shader3D_with_texture, modelShader, plankTexture, plankVertTexture, ropeTexture, groundTexture, landscapeTexture, logWall, platform);

    // Inicijalizacija kamere
    camera = new Camera((float)g_windowWidth / (float)g_windowHeight);
    camera->setOrbitDistance(3.5f);

    // Kontrola FPS-a
    const double targetFPS = 75.0;
    const double frameTime = 1.0 / targetFPS;
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        double now = glfwGetTime();
        double dt = now - lastTime;
        if (dt < frameTime) {
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
        gameManager->calculateScore();

        // Boja pozadine ukoliko se ne ucita tekstura
        glClearColor(0.1f, 0.2f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Palimo GL_DEPTH_TEST za 3D rendering
        glEnable(GL_DEPTH_TEST);

        if (camera) {
            float rotationSpeed = 10.5f;
            camera->updateOrbit(rotationSpeed * (float)dt);
        }

        if (renderer3D && camera) {
			// 1. Crtamo cilindar pozadine bez face cullinga zato sto je unutrasnjost vidljiva
            glDisable(GL_CULL_FACE);
            renderer3D->drawCylinder3D(camera, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(5.0f, 5.0f, 5.0f), landscapeTexture);
            
			// 2. Crtamo tlo bez face cullinga zato sto je vidljiva samo gornja strana
            renderer3D->drawGround3D(camera);
            
			// 3. Palimo face culling za ostale objekte ukoliko je omogucen
            if (faceCullingEnabled) {
                glEnable(GL_CULL_FACE);
                glCullFace(GL_BACK);
            }
            
			// 4. Crtamo ostale 3D objekte
            renderer3D->drawLogWall(camera, glm::vec3(0.0f, -5.25f, 0.0f), 4.0f);
            renderer3D->renderPlatform(camera, glm::vec3(0.0f, -1.3f, 0.0f), 0.0f);
            renderer3D->drawGallows3D(camera);
            renderer3D->drawHangman3D(camera, gameManager->getState().wrong);
        }

        // Gasimo GL_Depth_Test i Face Culling za 2D rendering
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        // Crtamo 2D UI
        if (uiRenderer) {
            uiRenderer->drawWord(gameManager->getState());
            uiRenderer->drawInputBuffer(inputBuffer, gameManager->getState().secret, wordMode);
            uiRenderer->drawTriedLetters(gameManager->getState().tried);
            uiRenderer->drawTriedWords(gameManager->getState().triedWords);
            uiRenderer->drawInstructions();
            drawStudentTexture();
            uiRenderer->drawScore(gameManager->getCurrentScore(), gameManager->getBestScore());
            uiRenderer->drawResult(gameManager->getState().finished, gameManager->getState().win);
        }

        if (keyboardManager)
            keyboardManager->drawKeyboard();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete gameManager;
    delete keyboardManager;
    delete textRenderer;
    delete uiRenderer;
    delete renderer3D;
    delete camera;
    delete logWall;
    delete platform;

    glfwTerminate();
    return 0;
}