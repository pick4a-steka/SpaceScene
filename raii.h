#pragma once

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stb_image.h>

// Класс для массива вершин
class VAO {
public:
    VAO();
    ~VAO();

    void bind() const;
    void unbind() const;
    GLuint getID() const;

private:
    GLuint id;
};

// Класс для вершинного буфера
class VBO {
public:
    VBO();
    ~VBO();

    void bind(GLenum target) const;
    void unbind(GLenum target) const;
    GLuint getID() const;

private:
    GLuint id;
};

// Класс для текстур
class Texture {
public:
    Texture(const std::string &filePath);
    ~Texture();

    void bind() const;
    void unbind() const;
    GLuint getID() const;

private:
    GLuint id;
};

// Класс для шейдеров
class Shader {
public:
    Shader(const std::string &vertexSource, const std::string &fragmentSource);
    ~Shader();

    void use() const;
    GLuint getID() const;

private:
    GLuint id;

    void checkCompileErrors(GLuint shader, const std::string &type);
};
