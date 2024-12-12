#include "raii.h"
#include "model.h"
#include "filesystem_getpath.h"
#include "camera.h"

#include <iostream>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define WIDTH 800
#define HEIGHT 600

void processInput(Camera &camera); // управление клавиатурой для перемещения камеры
float randomInRange(float min, float max);
glm::vec3 inclusion_pattern(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax);
void star_coordinate_generator(std::vector<glm::vec3> &coordinates);


float lastX = WIDTH / 2.0f, lastY = HEIGHT / 2.0f, deltaTime = 0.0f, lastFrame = 0.0f;
const int NUM_STARS = 150;
GLuint starTexture;
std::vector<glm::vec2> stars;

int main() {
    // Create an SFML window with OpenGL context
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    srand(time(0)); // устанавливаем seed для генерации случайных координат

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "OpenGL with SFML", sf::Style::Default, settings);

    if (!window.isOpen()) {
        std::cout << "Ошибка создания SFML окна" << std::endl;
        return -1;
    }

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    //generateStars();

    Shader ourShader("/home/mihal/QtTest/SpaceScene/model.vs", "/home/mihal/QtTest/SpaceScene/model.fs");
    Model star_model("/home/mihal/QtTest/SpaceScene/stars/scene.gltf");
    Model ourModel1("/home/mihal/QtTest/SpaceScene/spaceship/scene.gltf");
    Model ourModel2("/home/mihal/QtTest/SpaceScene/sun/scene.gltf");
    Model ourModel3("/home/mihal/QtTest/SpaceScene/celestial1/scene.gltf");
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

    std::vector<glm::vec3> coordinates;
    star_coordinate_generator(coordinates);

    // Hide the mouse cursor and set initial position
    window.setMouseCursorVisible(false);
    sf::Mouse::setPosition(sf::Vector2i(WIDTH / 2, HEIGHT / 2), window);

    bool running = true;
    sf::Clock clock;
    while (running) {
        float currentFrame = clock.getElapsedTime().asSeconds();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                running = false;
            }
            else if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }

        processInput(camera);

        // Get the current mouse position
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        float xOffset = mousePosition.x - lastX;
        float yOffset = lastY - mousePosition.y; // Invert yOffset for correct camera movement

        camera.ProcessMouseMovement(xOffset, yOffset);

        // Reset mouse position to the center of the window
        sf::Mouse::setPosition(sf::Vector2i(WIDTH / 2, HEIGHT / 2), window);

        // Clear the buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //drawStars(camera);

        // Use the shader
        ourShader.use();

        // Set view and projection matrices
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // Render the star_model
        for (unsigned int i = 0; i < NUM_STARS; ++i) {
            glm::mat4 star = glm::mat4(1.0f);
            star = glm::translate(star, coordinates[i]);
            star = glm::scale(star, glm::vec3(0.01f, 0.01f, 0.01f));
            ourShader.setMat4("model", star);
            star_model.Draw(ourShader);
        }

        // Render the model1
        glm::mat4 model1 = glm::mat4(1.0f);
        model1 = glm::translate(model1, glm::vec3(-10.0f, 0.0f, 0.0f));
        model1 = glm::scale(model1, glm::vec3(0.00025f, 0.00025f, 0.00025f));
        ourShader.setMat4("model", model1);
        ourModel1.Draw(ourShader);

        // Render the model2
        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, glm::vec3(0.0f, 0.0f, 0.0f));
        model2 = glm::scale(model2, glm::vec3(2.0f, 2.0f, 2.0f));
        ourShader.setMat4("model", model2);
        ourModel2.Draw(ourShader);

        // Render the model3
        glm::mat4 model3 = glm::mat4(1.0f);
        model3 = glm::translate(model3, glm::vec3(10.0f, 0.0f, 0.0f));
        model3 = glm::scale(model3, glm::vec3(0.2f, 0.2f, 0.2f));
        ourShader.setMat4("model", model3);
        ourModel3.Draw(ourShader);

        // Swap buffers
        window.display();
    }

    return 0;
}

void processInput(Camera &camera) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        camera.ProcessKeyboard(1, deltaTime);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        camera.ProcessKeyboard(2, deltaTime);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        camera.ProcessKeyboard(3, deltaTime);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        camera.ProcessKeyboard(4, deltaTime);
    }
}

// Генерация случайного числа в диапазоне [min, max]
float randomInRange(float min, float max) {
    return min + static_cast<float>(rand()) / (float)RAND_MAX * (max - min);
}

// схема случайного определения x, y, z
glm::vec3 inclusion_pattern(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax) {
    float x, y, z;

    x = randomInRange(xmin, xmax);
    y = randomInRange(ymin, ymax);
    z = randomInRange(zmin, zmax);

    return {x, y, z};
}

// составление вектора координа звезд
void star_coordinate_generator(std::vector<glm::vec3> &coordinates) {
    float xmin1 = -20.0f, xmax1 = 20.0f, ymin1 = -40.0f, ymax1 = -20.0f, zmin1 = -20.0f, zmax1 = 20.0f; // нижний куб
    float xmin2 = -20.0f, xmax2 = 20.0f, ymin2 = 20.0f, ymax2 = 40.0f, zmin2 = -20.0f, zmax2 = 20.0f; // верхний куб
    float xmin3 = -40.0f, xmax3 = -20.0f, ymin3 = -20.0f, ymax3 = 20.0f, zmin3 = -20.0f, zmax3 = 20.0f; // левый куб
    float xmin4 = 20.0f, xmax4 = 40.0f, ymin4 = -20.0f, ymax4 = 20.0f, zmin4 = -20.0f, zmax4 = 20.0f; // правый куб
    float xmin5 = -20.0f, xmax5 = 20.0f, ymin5 = -20.0f, ymax5 = 20.0f, zmin5 = 20.0f, zmax5 = 40.0f; // передний куб
    float xmin6 = -20.0f, xmax6 = 20.0f, ymin6 = -20.0f, ymax6 = 20.0f, zmin6 = -40.0f, zmax6 = -20.f; // задний куб

    for (unsigned int i = 0; i < NUM_STARS / 6; ++i) {
        coordinates.push_back(inclusion_pattern(xmin1, xmax1, ymin1, ymax1, zmin1, zmax1));
        coordinates.push_back(inclusion_pattern(xmin2, xmax2, ymin2, ymax2, zmin2, zmax2));
        coordinates.push_back(inclusion_pattern(xmin3, xmax3, ymin3, ymax3, zmin3, zmax3));
        coordinates.push_back(inclusion_pattern(xmin4, xmax4, ymin4, ymax4, zmin4, zmax4));
        coordinates.push_back(inclusion_pattern(xmin5, xmax5, ymin5, ymax5, zmin5, zmax5));
        coordinates.push_back(inclusion_pattern(xmin6, xmax6, ymin6, ymax6, zmin6, zmax6));
    }
}
