#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;

class Camera {
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed = 2.5f;
    float MouseSensitivity = 0.2f;
    float Zoom = 45.f;

    Camera();
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    glm::mat4 GetViewMatrix() const;

    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    void ProcessLeftMouseMovement(float xoffset, float yoffset);

    void ProcessRightMouseMovement(float xoffset, float yoffset) {};

    void ProcessMouseScroll(float yoffset);

private:
    void updateCameraVectors();
};

class Orbiter {
public:
    glm::vec3 Position;
    glm::vec3 LookAt;
    glm::vec3 Up;
    float Zoom = 45.f;

    float MovementSpeed = 2.5f;
    float MouseSensitivity = 0.02f;

    Orbiter(glm::vec3 position = glm::vec3(0.f, 0.f, 1.f), glm::vec3 lookAt = glm::vec3(0.f));
    Orbiter(float p_x, float p_y, float p_z, float l_x, float l_y, float l_z);

    glm::mat4 GetViewMatrix() const;

    void ProcessLeftMouseMovement(float xoffset, float yoffset);
    void ProcessRightMouseMovement(float xoffset, float yoffset);
    void ProcessMouseScroll(float yoffset);

private:
    void updateOrbiterVectors();

};

#endif