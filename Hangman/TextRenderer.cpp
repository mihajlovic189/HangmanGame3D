#include "TextRenderer.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

extern unsigned int createShader(const char* vsSource, const char* fsSource);

TextRenderer::TextRenderer()
    : textShader(0), textVAO(0), textVBO(0), 
      windowWidth(1920), windowHeight(1080), initialized(false) {
}

TextRenderer::~TextRenderer() {
    if (textVAO) glDeleteVertexArrays(1, &textVAO);
    if (textVBO) glDeleteBuffers(1, &textVBO);
}

bool TextRenderer::initialize(const char* fontPath, unsigned int fontSize, int winWidth, int winHeight) {
    windowWidth = winWidth;
    windowHeight = winHeight;
    
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
        return false;

    FT_Face face;
    const char* fontPaths[] = {
        "x64/Debug/arial.ttf",
        "../arial.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:\\Windows\\Fonts\\arial.ttf"
    };

    bool fontLoaded = false;
    for (int i = 0; i < 4; i++) {
        if (FT_New_Face(ft, fontPaths[i], 0, &face) == 0) {
            fontLoaded = true;
            break;
        }
    }

    if (!fontLoaded) {
        FT_Done_FreeType(ft);
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            continue;

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
            face->glyph->bitmap.width, face->glyph->bitmap.rows,
            0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            (int)face->glyph->bitmap.width,
            (int)face->glyph->bitmap.rows,
            face->glyph->bitmap_left,
            face->glyph->bitmap_top,
            (unsigned int)face->glyph->advance.x
        };
        characters.insert(std::pair<char, Character>(c, character));
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

    textShader = createShader("text.vert", "text.frag");
    if (textShader) {
        glUseProgram(textShader);
        glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f);
        glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    }

    initialized = true;
    return true;
}

void TextRenderer::updateWindowSize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    
    if (textShader) {
        glUseProgram(textShader);
        glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f);
        glUniformMatrix4fv(glGetUniformLocation(textShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    }
}

void TextRenderer::ndcToScreen(float ndcX, float ndcY, float &screenX, float &screenY, int width, int height) {
    screenX = (ndcX + 1.0f) * 0.5f * width;
    screenY = (1.0f - ndcY) * 0.5f * height;
}

void TextRenderer::renderText(const std::string &text, float ndcX, float ndcY, float scale, float r, float g, float b) {
    if (!textShader || characters.empty())
        return;

    float screenX, screenY;
    ndcToScreen(ndcX, ndcY, screenX, screenY, windowWidth, windowHeight);

    glUseProgram(textShader);
    glUniform3f(glGetUniformLocation(textShader, "textColor"), r, g, b);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textVAO);

    float x = screenX;
    float y = screenY;

    for (char c : text) {
        if (characters.find(c) == characters.end())
            continue;

        Character ch = characters[c];

        float xpos = x + ch.BearingX * scale;
        float ypos = y + (ch.SizeY - ch.BearingY) * scale;

        float w = ch.SizeX * scale;
        float h = ch.SizeY * scale;

        float vertices[6][4] = {
            { xpos,     ypos - h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos,     ypos - h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos - h,   1.0f, 0.0f }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::drawLetter(char c, float x, float y, float cellW, float cellH, float a) {
    float scale = cellH * 10.0f;
    float r = a, g = a, b = a;
    std::string s(1, c);
    renderText(s, x + 0.02f, y + 0.04f, scale, r, g, b);
}
