#include "mesh.h"
#include

Mesh::Mesh(const std::vector<Vertex> &vertices, \
           const std::vector<unsigned int> &indices, const std::vector<Txtr> &textures)
    : vertices(vertices), indices(indices), textures(textures) {
    setupMesh();
}

void Mesh::setupMesh() {
    vao.bind(); // привязываем VAO

    // загружаем вершины в VBO
    vbo.bind(GL_ARRAY_BUFFER);
    vbo.setData(GL_ARRAY_BUFFER, &vertices[0], vertices.size() * sizeof(Vertex), GL_STATIC_DRAW);

    // загружаем индексы в EBO
    ebo.bind(GL_ELEMENT_ARRAY_BUFFER);
    ebo.setData(GL_ELEMENT_ARRAY_BUFFER, &indices[0], indices.size() * sizeof(unsigned int), GL_STATIC_DRAW);

    // позиции вершин
    vao.setVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // нормали вершин
    vao.setVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // текстурные координаты вершин
    vao.setVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    vao.unbind(); // отвязывем VAO
}

void Mesh::draw(Shader &shader) {
    // Привязываем текстуры
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;

    for (unsigned int i = 0; i < textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0 + i); // активируем текстурный блок, до привязки
        // получаем номер текстуры
        std::stringstream ss;
        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse") {
            ss << diffuseNr++; // передаем unsigned int в stream
        }
        else if (name == "texture_specular") {
            ss << specularNr++; // передаем unsigned int в stream
        }
        number = ss.str();

        shader.setFloat(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    // отрисовываем полигональную сетку
    vao.bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    vao.unbind();
}
