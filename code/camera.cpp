#include "camera.hpp"

Camera::Camera(glm::vec3 position = glm::vec3(0.f), glm::vec3 up = glm::vec3(0.f, 1.f, 0.f), float yaw = YAW, float pitch = PITCH) {
    Position = position;
    Up = up;
    Yaw = yaw;
    Pitch = pitch;

    updateCameraVectors();
}

Camera::Camera(float posX = 0.f, float posY = 0.f, float posZ = 0.f, float upX = 1.f, float upY = 1.f, float upZ = 1.f, float yaw = YAW, float pitch = PITCH) {
    Position = glm::vec3(posX, posY, posZ);
    Up = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;

    updateCameraVectors();
}


glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    glm::vec3 right = glm::cross(Front, Up);
    switch (direction) {
    case FORWARD:
        Position += velocity * Front;
        break;
    case BACKWARD:
        Position -= velocity * Front;
        break;
    case LEFT:
        Position -= velocity * right;
        break;
    case RIGHT:
        Position += velocity * right;
        break;
    default:
        break;
    }
}

void Camera::ProcessLeftMouseMovement(float xoffset, float yoffset) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    if (Pitch > 89.0f)
        Pitch = 89.0f;
    if (Pitch < -89.0f)
        Pitch = -89.0f;

    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) {
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void Camera::updateCameraVectors() {
    Front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front.y = sin(glm::radians(Pitch));
    Front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(Front);
    Up    = glm::normalize(Up);
}

// ORBITER ---------------------------------------------------------------------

Orbiter::Orbiter(glm::vec3 position, glm::vec3 lookAt) : Position(position), LookAt(lookAt) {
    updateOrbiterVectors();
}

Orbiter::Orbiter(float p_x, float p_y, float p_z, float l_x, float l_y, float l_z) : Orbiter(glm::vec3(p_x, p_y, p_z), glm::vec3(l_x, l_y, l_z)) {
    updateOrbiterVectors();
}


glm::mat4 Orbiter::GetViewMatrix() const {
    return glm::lookAt(Position, LookAt, Up);
}

void Orbiter::ProcessLeftMouseMovement(float xoffset, float yoffset) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    glm::vec3 lookDir = Position - LookAt;
    glm::mat4x4 x_rotate = glm::rotate(glm::mat4x4(1.f), -yoffset, glm::normalize(glm::cross(lookDir, glm::vec3(0.f, 1.f, 0.f))));
    glm::vec3 Position_Temp = glm::vec3(x_rotate * glm::vec4(Position, 0.f));

    float sign_x = glm::dot(Position_Temp, glm::vec3(1.f, 0.f, 0.f));
    sign_x *= glm::dot(Position, glm::vec3(1.f, 0.f, 0.f));
    float sign_z = glm::dot(Position_Temp, glm::vec3(0.f, 0.f, 1.f));
    sign_z *= glm::dot(Position, glm::vec3(0.f, 0.f, 1.f));
    //std::cout << sign_x << " " << sign_z << std::endl;
    if (sign_x > 0 && sign_z > 0) Position = Position_Temp;

    glm::mat4x4 y_rotate = glm::rotate(glm::mat4x4(1.f), -xoffset, glm::vec3(0.f, 1.f, 0.f));
    Position = glm::vec3(y_rotate * glm::vec4(Position, 0.f));

    updateOrbiterVectors();
}

void Orbiter::ProcessRightMouseMovement(float xoffset, float yoffset) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    glm::vec3 lookDir = Position - LookAt;
    glm::vec3 ViewPlanX = glm::normalize(glm::cross(lookDir, Up));
    glm::vec3 ViewPlanY = glm::normalize(glm::cross(lookDir, ViewPlanX));

    LookAt += xoffset * ViewPlanX + yoffset * ViewPlanY;

    updateOrbiterVectors();
}

void Orbiter::ProcessMouseScroll(float yoffset) {
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}


void Orbiter::updateOrbiterVectors() {
    glm::vec3 lookDir = LookAt - Position;
    Up = glm::cross(lookDir, glm::vec3(0.f, 1.f, 0.f));
    Up = glm::cross(Up, lookDir);
    Up = glm::normalize(Up);
}


//EOF