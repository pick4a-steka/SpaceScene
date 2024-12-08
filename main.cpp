#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

int main() {
    // Создание окна
    sf::Window window(sf::VideoMode(640, 480), "SpaceScene", sf::Style::Default, sf::ContextSettings(32));
    window.setVerticalSyncEnabled(true);

    // Активация окна
    window.setActive(true);

    // Загрузка ресурсов, инициализация компонентов OpenGL, ...

    // запуск основного цикла
    bool running = true;
    while (running) {
        // Обработка событий
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                // Завершение работы программы
                running = false;
            }
            else if (event.type == sf::Event::Resized) {
                // регулировка viewport при изменении размера окна
                glViewport(0, 0, event.size.width, event.size.height);
            }
        }

        // Очистка буфферов
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // Отрисовка...


        // Конец текущего кадра (внутренняя замена переднего и заднего буферов)
        window.display();
    }

    // высвобождение ресурсов...


    return 0;
}
