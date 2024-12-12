#pragma once

#include "raii.h"
#include "mesh.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
//#include <SOIL/SOIL.h>

class Model {
public:
    // Методы
    Model(const std::string &modelPath);

    void Draw(Shader &shader);

private:
    // Данные модели
    std::vector<Mesh> meshes;
    std::vector<Txtr> textures_loaded;
    std::string directory;

    // Методы
    void loadModel(const std::string &modelPath);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Txtr> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    unsigned int TextureFromFile(const char *path, const std::string &directory);
};
