#include "raii.h"

// ===== VAO =====

VAO::VAO() {
    glGenVertexArrays(1, &id);
}

VAO::~VAO() {
    glDeleteVertexArrays(1, &id);
}

void VAO::bind() const {
    glBindVertexArray(id);
}

void VAO::unbind() const {
    glBindVertexArray(0);
}

GLuint VAO::getID() const {
    return id;
}

// ===============

// ===== VBO =====

VBO::VBO() {
    glGenBuffers(1, &id);
}

VBO::~VBO() {
    glDeleteBuffers(1, &id);
}

void VBO::bind(GLenum target) const {
    glBindBuffer(target, id);
}

void VBO::unbind(GLenum target) const {
    glBindBuffer(target, 0);
}

GLuint VBO::getID() const {
    return id;
}

// ==============

// ===== Texture =====

Texture::Texture(const std::string &filePath) {
    glGenTextures(1, &id); // генерация уникального идентификатора для текстуры на GPU
    bind();// glBindTexture(GL_TEXTURE_2D, id); // "привязка" текстуры, делая её текущей для всех операций, связанных с текстурами

    int width, height, channels;
    /*
     * stbi_load - функция из библиотеки stb_image для загрузки изображений.
     * filePath.c_str() - путь к файлу текстуры
     * width, height, channels - получают информацию о размере изображения и количестве цветовых каналов
     * 0 - указывает, что мы хотим загрузить изображение в его исходном формате
     * Результат:
     *  Если загрузка успешна, функция возвращает указатель на массив байтов (data),
     *  который содержит сырые пиксельные данные. В случае неудачной загрузки возвращает
     *  nullptr
    */
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
    if (data) {
        // загрузка данных изображения в текущую текстуру
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        /*
         * автоматически создает последовательность текстур меньших
         * размеров для текущей текстуры. Эти mipmaps используются
         * для оптимизации рендеринга объектов, которые находятся
         * далеко от камеры
        */
        glGenerateMipmap(GL_TEXTURE_2D);

        // освобождение памяти CPU: после загрузки текстуры в GPU память для изображения на CPU больше не нужна
        stbi_image_free(data); // освобождение памяти
    }
    else {
        std::cerr << "Ошибка загрузки текстуры: " << filePath << std::endl;
    }

    unbind(); // glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
    glDeleteTextures(1, &id);
}

void Texture::bind() const {
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture::getID() const {
    return id;
}

// ===============

// ===== Shader =====

Shader::Shader(const std::string& vertexSource, const std::string &fragmentSource) {
    // Создание и компиляция вершинного шейдера
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char *vShaderCode = vertexSource.c_str();
    glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");

    // Создание и компиляция фрагментного шейдера
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fShaderCode = fragmentSource.c_str();
    glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentShader);
    checkCompileErrors(fragmentShader, "FRAGMENT");

    // Линковка программы
    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);
    checkCompileErrors(id, "PROGRAM");

    // Уничтожение промежуточных шейдеров
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() {
    glDeleteProgram(id);
}

void Shader::use() const {
    glUseProgram(id);
}

GLuint Shader::getID() const {
    return id;
}

void Shader::checkCompileErrors(GLuint shader, const std::string &type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "Shader Compilation Error (" << type << "):\n" << infoLog << std::endl;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "Program Linking Error:\n" << infoLog << std::endl;
        }
    }
}
