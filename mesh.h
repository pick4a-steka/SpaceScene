#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/postprocess.h>


#include "raii.h"

// Определение структуры вершины
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

// Структура для хранения текстурных данных
struct Txtr {
    unsigned int id;
    std::string type;
    aiString path;
};

class Mesh {
public:
    Mesh(const std::vector<Vertex> &vertices, \
         const std::vector<unsigned int> &indices, const std::vector<Txtr> &textures);

    void draw(Shader &shader);

private:
    // Данные сетки
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Txtr> textures;

    // OpenGL объекты с автогенерацией буфера
    VAO vao;
    VBO vbo;
    EBO ebo;

    // Метод для настройки буферов
    void setupMesh();
};
