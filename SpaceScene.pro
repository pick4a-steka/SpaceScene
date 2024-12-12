TEMPLATE = app
CONFIG += c++17
CONFIG += core gui
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        camera.cpp \
        main.cpp \
        mesh.cpp \
        model.cpp \
        raii.cpp

LIBS += \
        -L./usr/local/SFML-2.6.2/lib/ -lsfml-graphics -lsfml-window -lsfml-system \
        -L/usr/local/assimp-master/bin/ -lassimp \
        -lglfw -lGLEW -lGL

INCLUDEPATH += \
        /usr/local/SFML-2.6.2/include/SFML \
        /usr/local/assimp-master/include/ \
        /usr/local/soil/include/

HEADERS += \
    camera.h \
    filesystem_getpath.h \
    mesh.h \
    model.h \
    raii.h \
    stb_image.h

DISTFILES += \
    model.fs \
    model.vs \
    spaceship/license.txt \
    spaceship/scene.bin \
    spaceship/scene.gltf \
    spaceship/textures/Ship_baseColor.png \
    spaceship/textures/Ship_emissive.png
