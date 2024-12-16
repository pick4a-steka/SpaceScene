#include "raii.h"
#include "model.h"
#include "filesystem_getpath.h"
#include "camera.h"

#include <iostream>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define WIDTH 800
#define HEIGHT 600

// Функция для управления камерой
void processInput(Camera &camera);

// Функции для размещения звезд в сцене
float randomInRange(float min, float max);
glm::vec3 inclusion_pattern(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax);
void star_coordinate_generator(std::vector<glm::vec3> &coordinates);

// Функция для обновления углов орбит и углов вращения
void update_move_object();

// Функции для управления кораблем
void handleInput();
void updateShip(Camera &camera);

// Функция для проверки коллизий
bool checkCollision(const glm::vec3& position1, float radius1, const glm::vec3& position2, float radius2);

// Функции для управления музыкой
void musicManager(std::vector<std::unique_ptr<sf::Music>> &musics);
void loadMusic(std::vector<std::unique_ptr<sf::Music>> &musics, const std::vector<std::string> &filePaths);

// Переменные для работы с мышью
float lastX = WIDTH / 2.0f, lastY = HEIGHT / 2.0f, deltaTime = 0.0f, lastFrame = 0.0f;

// Константа определяющая количество звезд
const int NUM_STARS = 150;

// Установка параметров фонового света
float ambientLightColor_x = 1.0f, ambientLightColor_y = 1.0f, ambientLightColor_z = 1.0f; // Белый свет
float ambientIntensity = 0.25f; // Интенсивность света
float specularIntensuty = 0.5f; // Интенсивность блика

// Переменные для управления вращением Европы
const float EuropaRotationSpeed = 1.5f;
float europa_angle = 0.0f;
float europa_orbit_angle = 0.0f;
const float EuropaOrbitSpeed = 3.0f;
const float EuropaRadiusOrbit = 10.0f;

// Переменные для управления вращением Ганимеда
const float HanimedRotationSpeed = 2.0f;
float hanimed_angle = 0.0f;
float hanimed_orbit_angle = 0.0f;
const float HanimedOrbitSpeed = 5.0f;
const float HanimedRadiusOrbit = 17.0f;

// Переменные для управления врещением Звезды
const float SunRotationSpeed = 1.7f;
float sun_angle = 0.0f;

// Переменные для управлением вращения астеройда по эллиптической орбите
const float a = 7.0f; // большая полуось
const float b = 5.0f; // малая полуось
float e = sqrt(1 - (b * b) / (a * a)); // эксцентриситет орбиты
float theta = 0.0f; // угол на орбите
float asteroid_angle = 0.0f;
const float AsteroidRotationSpeed = 30.0f;
const float scaleFactor = 10.0f;
const float safeDistance = 3.0f;

// Переменные для управления кораблем
glm::vec3 shipPosition = glm::vec3(0.0f, 0.0f, 3.0f); // позиция
glm::vec3 shipForward = glm::vec3(0.0f, 0.0f, -1.0f); // направление вперед
glm::vec3 shipUp = glm::vec3(0.0f, 1.0f, 0.0f); // вверх
glm::vec3 shipRight = glm::normalize(glm::cross(shipForward, shipUp)); // вправо
float shipSpeed = 0.0f; // скорость
float speedUp = 0.0f; // скорость подъема и спуска
glm::vec3 cameraOffset = glm::vec3(0.0f, 1.0f, 2.0f);
float falcon_angle = 0; // переменная для поворота
float falcon_slant = 0; // переменная для наклона
glm::vec3 cameraTarget = glm::vec3(0.0f);
glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 shipForwardTarget = shipForward;
const float friction = 0.98f; // коэффициент постепенного снижения скорости
const float acceleration = 2.0f; // ускорение
const float turnSpeed = 1.25f; // скорость поворота
float shipRadius = 1.0f;
std::vector<glm::vec3> obstaclePos(5, glm::vec3(0.0f, 0.0f, 0.0f)); // позиции объектов
std::vector<float> objects_radius = {1.8f, 0.8f, 0.1f, 1.3f, 1.4f}; // вектор с радиусами объектов
// звезда, звезда смерти, астероид, Европа, Ганимед

int main() {
    // Create an SFML window with OpenGL context
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = 4;
    settings.majorVersion = 3;
    settings.minorVersion = 3;

    srand(time(0)); // устанавливаем seed для генерации случайных координат

    std::cout << "e: " << e << std::endl;

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

    // Загрузка шейдеров
    Shader ourShaderForObjects("/home/mihal/QtTest/SpaceScene/model.vsh", "/home/mihal/QtTest/SpaceScene/model.fsh");
    Shader shaderForLampObject("/home/mihal/QtTest/SpaceScene/model.vsh", "/home/mihal/QtTest/SpaceScene/lamp_model.fsh");
    Shader shaderForSpecularObj("/home/mihal/QtTest/SpaceScene/model.vsh", "/home/mihal/QtTest/SpaceScene/specular_model.fsh");

    // Загрузка моделей
    Model star_model("/home/mihal/QtTest/SpaceScene/stars/scene.gltf");
    Model millennium_falcon("/home/mihal/QtTest/SpaceScene/spaceship/scene.gltf");
    Model sun("/home/mihal/QtTest/SpaceScene/sun/scene.gltf");
    Model death_star("/home/mihal/QtTest/SpaceScene/death_star/scene.gltf");
    Model hanimed("/home/mihal/QtTest/SpaceScene/celestial1/scene.gltf");
    Model asteroid("/home/mihal/QtTest/SpaceScene/asteroid/scene.gltf");
    Model europa("/home/mihal/QtTest/SpaceScene/europa/scene.gltf");
    Camera camera(cameraOffset, glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);

    obstaclePos[0] = {0.0f, 0.0f, 0.0f}; // позиция звезды
    obstaclePos[1] = {0.0f, 0.0f, -13.0f}; // позиция "звезды смерти"

    // Загрузка музыки
    std::vector<std::string> musicFiles = {
            "/home/mihal/QtTest/SpaceScene/music/star_wars_maintheme.ogg",
            "/home/mihal/QtTest/SpaceScene/music/cantina_band.ogg",
            "/home/mihal/QtTest/SpaceScene/music/Dart_Veider_theme.ogg",
            "/home/mihal/QtTest/SpaceScene/music/fun_theme.ogg"
        };

    std::vector<std::unique_ptr<sf::Music>> musics;
    loadMusic(musics, musicFiles);

    // Установка параметров музыки
    for (auto &music : musics) {
        music->setVolume(50);
        music->setLoop(true);
    }

    musics[0]->play(); // Запускаем первый трек

    // Установка координат для звезд
    std::vector<glm::vec3> coordinates;
    star_coordinate_generator(coordinates);

    // Прячем курсор мыши и устанавливаем камеру по центру экрана
    window.setMouseCursorVisible(false);
    sf::Mouse::setPosition(sf::Vector2i(WIDTH / 2, HEIGHT / 2), window);

    bool running = true;
    sf::Clock clock;
    while (running) {
        float currentFrame = clock.getElapsedTime().asSeconds();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        update_move_object();

        // вычисляем радиус в данной точке орбиты
        float r = a * (1 - e * e) / (1 + e * cos(glm::radians(theta))) + safeDistance;
        float x = r * cos(glm::radians(theta));
        float z = r * sin(glm::radians(theta));

        //std::cout << "r: " << r << std::endl;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                running = false;
            }
            else if (event.type == sf::Event::Resized) {
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }

        // Подключение управления клавишами
        // processInput(camera);
        handleInput();
        updateShip(camera);

        // Подключение менеджера управлением музыкой
        musicManager(musics);

        // Получаем текущее положение мыши
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        float xOffset = mousePosition.x - lastX;
        float yOffset = lastY - mousePosition.y; // Invert yOffset for correct camera movement

        camera.ProcessMouseMovement(xOffset, yOffset);

        // Переустанавливаем позицию мыши на центр экрана
        sf::Mouse::setPosition(sf::Vector2i(WIDTH / 2, HEIGHT / 2), window);

        // Чистим буферы
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Подключаем шейдер для объектов источников света
        shaderForLampObject.use();

        // Устанавливаем матрицы вида и проекции
        glm::mat4 projection_lampObg = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
        glm::mat4 view_lampObj = camera.GetViewMatrix();
        //glm::mat4 view_lampObj = camera.GetViewMatrix(cameraTarget);
        shaderForLampObject.setMat4("projection", projection_lampObg);
        shaderForLampObject.setMat4("view", view_lampObj);

        // Рендер звезд
        for (unsigned int i = 0; i < NUM_STARS; ++i) {
            glm::mat4 star = glm::mat4(1.0f);
            star = glm::translate(star, coordinates[i]);
            star = glm::scale(star, glm::vec3(0.01f, 0.01f, 0.01f));
            shaderForLampObject.setMat4("model", star);
            star_model.Draw(shaderForLampObject);
        }

        // Рендер "Солнца"
        glm::mat4 model_sun = glm::mat4(1.0f);
        model_sun = glm::translate(model_sun, glm::vec3(0.0f, 0.0f, 0.0f));
        model_sun = glm::scale(model_sun, glm::vec3(2.0f, 2.0f, 2.0f));
        model_sun = glm::rotate(model_sun, glm::radians(sun_angle), glm::vec3(0.0f, 1.0f, 0.0f));
        shaderForLampObject.setMat4("model", model_sun);
        sun.Draw(shaderForLampObject);

        // Подключаем шейдер для обычных объектов с бликами
        shaderForSpecularObj.use();

        // Передаем параметры фонового света шейдер и блика
        shaderForSpecularObj.setVec3("ambient_light_color", ambientLightColor_x, ambientLightColor_y, ambientLightColor_z);
        shaderForSpecularObj.setFloat("ambient_intensity", ambientIntensity);
        shaderForSpecularObj.setVec3("lightPos", 0.0f, 0.0f, 0.0f);
        shaderForSpecularObj.setFloat("specular_intensity", specularIntensuty);
        shaderForSpecularObj.setVec3("viewPos", camera.Position.x, camera.Position.y, camera.Position.z);

        // Устанавливаем матрицы вида проекции
        glm::mat4 projection_specular = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
        glm::mat4 view_specular = camera.GetViewMatrix();
        //glm::mat4 view_specular = camera.GetViewMatrix(cameraTarget);
        ourShaderForObjects.setMat4("projection", projection_specular);
        ourShaderForObjects.setMat4("view", view_specular);

        // Рендер "Звезды Смерти"
        glm::mat4 model_death_star = glm::mat4(1.0f);
        model_death_star = glm::translate(model_death_star, glm::vec3(0.0f, 0.0f, -13.0f));
        model_death_star = glm::scale(model_death_star, glm::vec3(0.03f, 0.03f, 0.03f));
        model_death_star = glm::rotate(model_death_star, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        shaderForSpecularObj.setMat4("model", model_death_star);
        death_star.Draw(shaderForSpecularObj);

        // Подключаем шейдер для обычных объектов
        ourShaderForObjects.use();

        // Передаем параметры фонового света в шейдер
        ourShaderForObjects.setVec3("ambient_light_color", ambientLightColor_x, ambientLightColor_y, ambientLightColor_z);
        ourShaderForObjects.setFloat("ambient_intensity", ambientIntensity);
        ourShaderForObjects.setVec3("lightPos", 0.0f, 0.0f, 0.0f);

        // Устанавливаем матрицы вида проекции
        glm::mat4 projection_obj = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.f);
        glm::mat4 view_obj = camera.GetViewMatrix();
        //glm::mat4 view_obj = camera.GetViewMatrix(cameraTarget);
        ourShaderForObjects.setMat4("projection", projection_obj);
        ourShaderForObjects.setMat4("view", view_obj);        

        // Рендер астеройда
        glm::mat4 model_asteroid = glm::mat4(1.0f);
        model_asteroid = glm::translate(model_asteroid, glm::vec3(x, 0.0f, z));
        //std::cout << "Theta: " << theta << " x: " << x << " z: " << z << std::endl;
        model_asteroid = glm::scale(model_asteroid, glm::vec3(0.2f, 0.2f, 0.2f));
        model_asteroid = glm::rotate(model_asteroid, glm::radians(asteroid_angle), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShaderForObjects.setMat4("model", model_asteroid);
        asteroid.Draw(ourShaderForObjects);
        obstaclePos[2] = {x, 0.0f, z};

        // Рендер Европы
        glm::mat4 model_europa = glm::mat4(1.0f);
        model_europa = glm::rotate(model_europa, glm::radians(europa_orbit_angle), glm::vec3(0.0f, 1.0f, 0.0f));
        model_europa = glm::translate(model_europa, glm::vec3(EuropaRadiusOrbit, 0.0f, 0.0f));
        model_europa = glm::scale(model_europa, glm::vec3(0.002f, 0.002f, 0.002f));
        model_europa = glm::rotate(model_europa, glm::radians(europa_angle), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShaderForObjects.setMat4("model", model_europa);
        europa.Draw(ourShaderForObjects);
        obstaclePos[3] = glm::vec3(model_europa[3]);

        // Рендер "Сокола тысячелетия"
        glm::mat4 model_falcon = glm::mat4(1.0f);
        model_falcon = glm::translate(model_falcon, shipPosition);
        model_falcon[0] = glm::vec4(shipRight, 0.0f); // направление вправо
        model_falcon[1] = glm::vec4(shipUp, 0.0f); // направление вверх
        model_falcon[2] = glm::vec4(-shipForward, 0.0f); // направление вперед
        model_falcon = glm::scale(model_falcon, glm::vec3(0.00025f, 0.00025f, 0.00025f));
        model_falcon = glm::rotate(model_falcon, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShaderForObjects.setMat4("model", model_falcon);
        millennium_falcon.Draw(ourShaderForObjects);

        // Рендер "Ганимеда"
        glm::mat4 model_hanimed = glm::mat4(1.0f);
        model_hanimed = glm::rotate(model_hanimed, glm::radians(hanimed_orbit_angle), glm::vec3(0.0f, 1.0f, 0.0f));
        model_hanimed = glm::translate(model_hanimed, glm::vec3(HanimedRadiusOrbit, 0.0f, 0.0f));
        model_hanimed = glm::scale(model_hanimed, glm::vec3(0.2f, 0.2f, 0.2f));
        model_hanimed = glm::rotate(model_hanimed, glm::radians(hanimed_angle), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShaderForObjects.setMat4("model", model_hanimed);
        hanimed.Draw(ourShaderForObjects);
        obstaclePos[4] = glm::vec3(model_hanimed[3]);

        // Свап буферов
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

void update_move_object() {
    // расчет вращения Европы
    europa_angle += deltaTime * EuropaRotationSpeed;
    if (europa_angle > 360.0f) {
        europa_angle -= 360.0f;
    }

    europa_orbit_angle += deltaTime * EuropaOrbitSpeed;
    if (europa_orbit_angle > 360.0f) {
        europa_orbit_angle -= 360.0f;
    }

    // расчет вращения вокруг своей оси звезды
    sun_angle += deltaTime * SunRotationSpeed;
    if (sun_angle > 360.0f) {
        sun_angle -= 360.0f;
    }

    // расчет вращения Ганимеда вокруг своей оси
    hanimed_angle += deltaTime * HanimedRotationSpeed;
    if (hanimed_angle > 360.0f) {
        hanimed_angle -= 360.0f;
    }

    // расчет вращения Ганимеда по орбите
    hanimed_orbit_angle += deltaTime * HanimedOrbitSpeed;
    if (hanimed_orbit_angle > 360.0f) {
        hanimed_orbit_angle -= 360.0f;
    }

    // скорость астероида будет зависеть от экцентриситета и угла
    float angularSpeed = scaleFactor * sqrt((1 + e * cos(glm::radians(theta))) / (1 - e * e));

    //std::cout << "mult ang speed and delta time: " << angularSpeed * deltaTime << std::endl;
    // обновление угла
    theta += angularSpeed * deltaTime;
    //theta += 1.0f;
    if (theta > 360.0f) {
        theta -= 360.0f;
    }

    // обновляем угол вращения вокруг оси для астероида
    asteroid_angle += deltaTime * AsteroidRotationSpeed;
    if (asteroid_angle > 360.0f) {
        asteroid_angle -= 360.0f;
    }
}

void handleInput() {
    // ускорение/торможение
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        shipSpeed += acceleration * deltaTime;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        shipSpeed -= acceleration * deltaTime;
    }

    // Повороты
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        falcon_angle += turnSpeed * deltaTime;
        falcon_angle = glm::mod(falcon_angle, 360.0f); // Ограничение угла поворота

        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(falcon_angle), shipUp);
        shipForward = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(shipForward, 1.0f)));
        shipRight = glm::normalize(glm::cross(shipForward, shipUp));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && !sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        falcon_angle += turnSpeed * deltaTime;
        falcon_angle = glm::mod(falcon_angle, 360.0f); // Ограничение угла поворота

        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-falcon_angle), shipUp);
        shipForward = glm::normalize(glm::vec3(rotationMatrix * glm::vec4(shipForward, 1.0f)));
        shipRight = glm::normalize(glm::cross(shipForward, shipUp));
    }

    // Подъем/спуск
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        speedUp += (acceleration * deltaTime) / 1.2f;
    }
    if (sf::Keyboard::isKeyPressed((sf::Keyboard::LShift))) {
        speedUp -= (acceleration * deltaTime) / 1.2f;
    }
}

bool checkCollision(const glm::vec3& position1, float radius1, const glm::vec3& position2, float radius2) {
    float distance = glm::length(position1 - position2);
    // std::cout << "Расстояние: " << distance << std::endl;
    return distance < (radius1 + radius2);
}

void updateShip(Camera &camera) {
    // std::cout << "updateShip" << std::endl;
    shipPosition += shipForward * shipSpeed * deltaTime; // движение вперед/назад
    shipPosition += shipUp * speedUp * deltaTime; // Подъем/спуск

    /*for (unsigned int i = 0; i < obstaclePos.size(); ++i) {
        std::cout << i << ". pos: " << "{" << obstaclePos[i].x << ", " << obstaclePos[i].y << ", " << obstaclePos[i].z << "}" << std::endl;
    }*/

    // Проверяем столкновение с каждым объектом
    for (unsigned int i = 0; i < obstaclePos.size(); ++i) {
        if (checkCollision(shipPosition, shipRadius, obstaclePos[i], objects_radius[i])) {
            // std::cout << "Случилась коллизия" << std::endl;
            // столкновение
            shipSpeed = 0.0f;
            speedUp = 0.0f;
            shipPosition = glm::vec3(0.0f, 0.0f, 3.0f);
            return;
        }
    }

    // Постепенное снижение скорости
    shipSpeed *= friction;
    speedUp *= 0.9f;

    // Убедимся, что векторы остаются ортогональными
    shipRight = glm::normalize(glm::cross(shipForward, shipUp));
    shipUp = glm::normalize(glm::cross(shipRight, shipForward));

    // Обновление камеры
    cameraTarget = shipPosition + shipForward;
    camera.Position = shipPosition - shipForward * cameraOffset.z + worldUp * cameraOffset.y;
}

void loadMusic(std::vector<std::unique_ptr<sf::Music>> &musics, const std::vector<std::string> &filePaths) {
    for (const auto &file : filePaths) {
        auto music = std::make_unique<sf::Music>();
        if (!music->openFromFile(file)) {
            std::cerr << "Ошибка загрузки музыки: " << file << std::endl;
            exit(-1);
        }
        musics.push_back(std::move(music));
    }
}

void musicManager(std::vector<std::unique_ptr<sf::Music>> &musics) {
    // Проверяем нажатие клавиш и переключаем музыку
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
        for (auto &music : musics) {
            music->stop(); // Останавливаем всю музыку
        }
        musics[0]->play(); // Воспроизводим первый трек
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
        for (auto &music : musics) {
            music->stop();
        }
        musics[1]->play();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
        for (auto &music : musics) {
            music->stop();
        }
        musics[2]->play();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) {
        for (auto &music : musics) {
            music->stop();
        }
        musics[3]->play();
    }
}
