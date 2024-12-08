#include "model.h"

Model::Model(const std::string &modelPath) {
    loadModel(modelPath);
}

void Model::Draw(Shader &shader) {
    for (unsigned int i = 0; i < meshes.size(); ++i) {
        meshes[i].draw(shader);
    }
}

void Model::loadModel(const std::string &modelPath) {
    // Загрузка модели с помощью Assimp
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs);

    // Проверка загруженной сцены
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Ошибка загрузки модели: " << importer.GetErrorString() << std::endl;
        return;
    }
    // из полного пути к файлу вычленяем путь к каталогу с ним
    directory = modelPath.substr(0, modelPath.find_last_of('/'));

    // обработка узлов сцены
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
    // Обрабатываем все полигональные сетки в узле(если они есть)
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // выполнить ту же обработку и для каждого потомка узла
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Txtr> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;

        // обработка координат вершин, нормалей и текстурных вершин
        glm::vec3 vector;
        // mVertices - массив положений вершин
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.normal = vector;

        if (mesh->mTextureCoords[0]) { // сетка обладает набором текстурных координат?
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;
        }
        else {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // обработка индексов
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // обработка материала
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Txtr> diffuseMaps = loadMaterialTextures(material, \
                                              aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Txtr> specularMaps = loadMaterialTextures(material, \
                                              aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Txtr> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
    std::vector<Txtr> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
        aiString str;
        mat->GetTexture(type, i, &str);
        Txtr texture;
        texture.id = TextureFromFile(str.C_Str(), directory);
        texture.type = typeName;
        texture.path = str;
        textures.push_back(texture);
    }

    return textures;
}
