#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
unsigned int createShader(const char* vsSource, const char* fsSource);
unsigned int loadImageToTexture(const char* filePath);
unsigned int loadImageToTextureWithTiling(const char* filepath, bool enableTiling = false);
GLFWcursor* loadImageToCursor(const char* filePath);