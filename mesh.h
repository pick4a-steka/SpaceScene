#pragma once

#include "raii.h"

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/postprocess.h>

#define MAX_BONE_INFLUENCE 4

// Определение структуры вершины
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
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

    // OpenGL объекты
    /*VAO vao;
    VBO vbo;
    EBO ebo;*/
    unsigned int VAO, VBO, EBO;

    // Метод для настройки буферов
    void setupMesh();
};
