#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "raii.h"
#include "mesh.h"

class Model {
public:
    // Методы
    Model(const std::string &modelPath);

    void Draw(Shader &shader);

private:
    // Данные модели
    std::vector<Mesh> meshes;
    std::string directory;

    // Методы
    void loadModel(const std::string &modelPath);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Txtr> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    unsigned int TextureFromFile(std::string str, std::string &directory);
};

/*class Model {
public:
    Model(const std::string &texturePath, const std::string &modelPath, \
          const std::string &vertexSource, const std::string &fragmentSource) :
        texture(texturePath), shader(vertexSource, fragmentSource) {
        loadModel(modelPath); // загрузка модели
    }
    virtual ~Model();

    // Трансформации
    void setPosition(const glm::vec3 &position);
    void setRotation(const glm::vec3 &rotation);
    void setScale(const glm::vec3 &scale);

    // Рендеринг
    virtual void draw(const glm::mat4 &view, const glm::mat4 &projection) = 0;
    virtual void update(float deltaTime) = 0; // для анимации

    void loadModel(const std::string &modelPath);

    // Рекурсивная обработка узлов
    void processNode(aiNode *node, const aiScene *scene);

    // Обработка меша
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

protected:
    // OpenGL объекты для рендера
    VAO vao;
    VBO vbo;

    Texture texture;
    Shader shader;

    glm::mat4 modelMatrix;

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

class StarModel : public Model {
public:
    StarModel();
    ~StarModel();

    void draw(const glm::mat4 &view, const glm::mat4 &projection) override;
    void update(float deltaTime) override;

private:

};

class PlanetModel : public Model {
public:
    PlanetModel();
    ~PlanetModel();


    void draw(const glm::mat4 &view, const glm::mat4 &projection) override;
    void update(float deltaTime) override;

private:

};

class SpaceshipModel : public Model {
public:
    SpaceshipModel();
    ~SpaceshipModel();

    void draw(const glm::mat4 &view, const glm::mat4 &projection) override;
    void update(float deltaTime) override;

private:

};*/
