#pragma once

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stb_image.h>
#include <vector>

// Класс для массива вершин
class VAO {
public:
    VAO();
    ~VAO();

    void bind() const;
    void unbind() const;
    GLuint getID() const;
    void setData();

    // настройка атрибутов вершин
    void setVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);

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

    // загрузка данных в буфер
    void setData(GLenum target, const void *data, GLsizeiptr size, GLenum usage) const;

private:
    GLuint id;
};

// Класс для текстур
class Texture {
public:
    Texture() {}
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
    Shader() {}
    Shader(const char *vertexPath, const char *fragmentPath);
    ~Shader();

    void use() const;
    GLuint getID() const;
    void setFloat(const std::string &name, float value) const;

private:
    GLuint id;

    void checkCompileErrors(GLuint shader, const std::string &type);
};

// Класс для массива индексов
class EBO {
public:
    EBO();
    EBO(const std::vector<GLuint> &indices); // создание EBO с данными
    ~EBO();

    EBO(EBO&& other) noexcept; // конструктор перемещения
    EBO& operator=(EBO&& other) noexcept; // оператор присваивания для перемещения

    void bind(GLenum target) const;
    void unbind(GLenum target) const;
    GLuint getID() const;
    void setData(GLenum target, const void *data, GLsizeiptr size, GLenum usage);

private:
    GLuint id;
};
