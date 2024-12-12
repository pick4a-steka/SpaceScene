#include <camera.h>

void Camera::ProcessKeyboard(int direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (direction == 1) { // прямо
        Position += Front * velocity;
    }
    if (direction == 2) { // назад
        Position -= Front * velocity;
    }
    if (direction == 3) { // влево
        Position -= Right * velocity;
    }
    if (direction == 4) { // вправо
        Position += Right * velocity;
    }
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch) {
    xOffset *= MouseSensitivity;
    yOffset *= MouseSensitivity;

    Yaw += xOffset;
    Pitch -= yOffset;

    if (constrainPitch) {
        if (Pitch > 89.0f) {
            Pitch = 89.0f;
        }
        if (Pitch < -89.0f) {
            Pitch = - 89.0f;
        }
    }

    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yOffset) {
    Zoom -= (float)yOffset;
    if (Zoom < 1.0f) {
        Zoom = 1.0f;
    }
    if (Zoom > 45.0f) {
        Zoom = 45.0f;
    }
}
