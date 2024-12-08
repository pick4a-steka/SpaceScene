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

void VAO::setVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) {
    glEnableVertexAttribArray(index); // включаем указанный массив атрибутов вершин
    glVertexAttribPointer(index, size, type, normalized, stride, pointer); // устанавливается указатель на чтение в буфере вершины
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

void VBO::setData(GLenum target, const void *data, GLsizeiptr size, GLenum usage) const {
    glBufferData(target, size, data, usage); // загружаем данные
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
    std::string vertexCode;
    std::string fragmentCode;

    // Загружаем код шейдеров из файлов
    std::ifstream vShaderFile(vertexSource);
    std::ifstream fShaderFile(fragmentSource);
    std::stringstream vShaderStream, fShaderStream;

    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();

    // Компиляция вершинного шейдера
    const char *vShaderCode = vertexCode.c_str();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShader);
    checkCompileErrors(vertexShader, "VERTEX");

    // Компиляция фрагментного шейдера
    const char *fShaderCode = fragmentCode.c_str();
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
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

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
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

// ===============

// ====== EBO =====

EBO::EBO() : id(0) {
    glGenBuffers(1, &id);
    if (id == 0) {
        std::cerr << "Error, EBO doesn't create" << std::endl;
    }
}

EBO::EBO(const std::vector<GLuint> &indices) : EBO() {
    bind(GL_ELEMENT_ARRAY_BUFFER);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    unbind(GL_ELEMENT_ARRAY_BUFFER);
}

EBO::~EBO() {
    if (id != 0) {
        glDeleteBuffers(1, &id);
    }
}

EBO::EBO(EBO&& other) noexcept : id(other.id) {
    other.id = 0; // обнуляем старый объект
}

EBO& EBO::operator=(EBO &&other) noexcept {
    if (this != &other) {
        // удаляем старый буфер
        if (id != 0) {
            glDeleteBuffers(1, &id);
        }

        // Перемещение
        id = other.id;
        other.id = 0;
    }

    return *this;
}

void EBO::bind(GLenum target) const {
    glBindBuffer(target, id);
}

void EBO::unbind(GLenum target) const {
    glBindBuffer(target, 0);
}

void EBO::setData(GLenum target, const void *data, GLsizeiptr size, GLenum usage) {
    glBufferData(target, size, data, usage);
}

GLenum EBO::getID() const {
    return id;
}

// ===============
