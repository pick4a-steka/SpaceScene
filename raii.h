#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stb_image.h>
#include <vector>

// Класс для шейдеров
class Shader {
public:
    Shader() {}
    Shader(const char *vertexPath, const char *fragmentPath);
    ~Shader();

    void use() const;
    GLuint getID() const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, const glm::mat4 &mat);

private:
    GLuint id;

    void checkCompileErrors(GLuint shader, const std::string &type);
};
