#include "Camera.hpp"

using namespace gd;

Camera::Camera(vec3 pos, vec3 rot, vec3 dir) : position(pos), rotation(rot), direction(dir)
{
    // initialize the right vector of the camera (it changes as the camera rotates)
    cameraRight = glm::vec3(
            sin(glm::radians(rotation.x) - 3.14f / 2.0f),
            0.f,
            cos(glm::radians(rotation.x) - 3.14f / 2.0f));
}

mat4 Camera::generateViewMatrix()
{
    // calculate the camera's facing direction
    direction = normalize(glm::vec3(
        cos(glm::radians(rotation.y)) * sin(glm::radians(rotation.x)),
        sin(glm::radians(rotation.y)),
        cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x))));

    // Recalculate the right vector relative to camera view
    cameraRight = normalize(glm::vec3(
        sin(glm::radians(rotation.x) - 3.14f / 2.0f),
        0,
        cos(glm::radians(rotation.x) - 3.14f / 2.0f)));

    // world Up : perpendicular to look direction and world right
    glm::vec3 cameraUp = normalize(glm::cross(cameraRight, direction));
    // glm::vec3 cameraUp = glm::normalize(glm::vec3(0, 1.f, 0));
    glm::vec3 cameraCenter = position + direction; // camera position combined with its face direction

    // GLFW function for generating a view matrix with our variables
    return glm::lookAt(position, cameraCenter, cameraUp);
}
