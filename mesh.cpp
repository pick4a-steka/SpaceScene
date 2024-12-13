#include "mesh.h"

Mesh::Mesh(const std::vector<Vertex> &vertices, \
           const std::vector<unsigned int> &indices, const std::vector<Txtr> &textures)
    : vertices(vertices), indices(indices), textures(textures) {
    if (vertices.empty() || indices.empty()) {
        std::cerr << "Error: Vertices or indices are empty!" << std::endl;
        return;
    }
    setupMesh();
}

void Mesh::setupMesh() {
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

    glBindVertexArray(0);
}

void Mesh::draw(Shader &shader) {
    // Привязываем текстуры
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    for (unsigned int i = 0; i < textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i); // активируем текстурный блок, до привязки
        // получаем номер текстуры
        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse") {
            number = std::to_string(diffuseNr++);
        }
        else if (name == "texture_specular") {
            number = std::to_string(specularNr++);
        }
        else if (name == "texture_normal") {
            number = std::to_string(normalNr++);
        }
        else if (name == "texture_height") {
            number = std::to_string(heightNr++);
        }

        // теперь установим сэмплер на правильную единицу измерения текстуры
        glUniform1i(glGetUniformLocation(shader.getID(), (name + number).c_str()), i);
        // и привяжем текстуру
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // отрисовываем полигональную сетку
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}
