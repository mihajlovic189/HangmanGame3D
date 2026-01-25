#include "Util.h"
#include <fstream>
#include <sstream>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static unsigned int compileShader(GLenum type, const char* path) {
    std::ifstream f(path);
    if(!f.is_open()) 
    { 
        std::cout << "Ne mogu da otvorim fajl: " << path << std::endl; 
        return 0; 
    }
    std::stringstream ss; ss << f.rdbuf();
    std::string src = ss.str();
    const char* csrc = src.c_str();
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader,1,&csrc,nullptr);
    glCompileShader(shader);
    int ok; 
    glGetShaderiv(shader, GL_COMPILE_STATUS,&ok);
    if(!ok)
    { 
        char log[1024]; 
        glGetShaderInfoLog(shader,1024,nullptr,log); 
        std::cout << "Sejder greska(" << path << "): " << log << std::endl; }
    return shader;
}

unsigned int createShader(const char* vsSource, const char* fsSource) {
    unsigned int vs = compileShader(GL_VERTEX_SHADER, vsSource);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fsSource);
    unsigned int prog = glCreateProgram();
    glAttachShader(prog, vs); 
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    int ok; 
    glGetProgramiv(prog, GL_LINK_STATUS,&ok);
    if(!ok)
    { 
        char log[1024]; 
        glGetProgramInfoLog(prog,1024,nullptr,log); 
        std::cout << "Link greska: " << log << std::endl; 
    }
    glDeleteShader(vs); glDeleteShader(fs);
    return prog;
}

unsigned int loadImageToTexture(const char* filePath){
    stbi_set_flip_vertically_on_load(1);
    int w,h,ch;
    unsigned char* data = stbi_load(filePath,&w,&h,&ch,0);
    if(!data)
    { 
        std::cout<<"Ne mogu ucitati sliku: "<<filePath<<std::endl; 
        return 0; 
    }
    GLenum fmt = ch==4?GL_RGBA: (ch==3?GL_RGB:GL_RED);
    unsigned int tex; glGenTextures(1,&tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D,0,fmt,w,h,0,fmt,GL_UNSIGNED_BYTE,data);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D,0);
    stbi_image_free(data);
    return tex;
}

unsigned int loadImageToTextureWithTiling(const char* filepath, bool enableTiling) {
    stbi_set_flip_vertically_on_load(1);
    int w, h, ch;
    unsigned char* data = stbi_load(filepath, &w, &h, &ch, 0);
    
    if (!data) {
        std::cout << "[ERROR] Failed to load texture: " << filepath << std::endl;
        return 0;
    }

    GLenum fmt = ch == 4 ? GL_RGBA : (ch == 3 ? GL_RGB : GL_RED);
    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);

    if (enableTiling) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);    
    return tex;
}

GLFWcursor* loadImageToCursor(const char* filePath) {
    int w, h, ch;
    unsigned char* data = stbi_load(filePath, &w, &h, &ch, 4);
    if (!data) {
        std::cout << "Ne mogu ucitati kursor sliku: " << filePath << std::endl;
        return nullptr;
    }

    for (int y = 0; y < h / 2; y++) {
        int top = y * w * 4;
        int bottom = (h - y - 1) * w * 4;

        for (int x = 0; x < w * 4; x++) {
            std::swap(data[top + x], data[bottom + x]);
        }
    }

    GLFWimage image;
    image.width = w;
    image.height = h;
    image.pixels = data;

    int hotspotX = w / 2;
    int hotspotY = h / 2;

    GLFWcursor* cursor = glfwCreateCursor(&image, hotspotX, hotspotY);

    stbi_image_free(data);

    return cursor;
}