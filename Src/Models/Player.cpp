#include "Player.hpp"

using namespace model;
using namespace glm;

Player::Player(std::string modelPath, std::string texturePath, vec3 color, vec3 pos, vec3 rot, vec3 sca) : Model3D(modelPath, texturePath, color, pos, rot, sca)
{
}

void Player::directionalMove(bool isForward)
{
    if (isForward)
    {
        position.x += sin(glm::radians(rotation.y)) * speed;
        position.z += cos(glm::radians(rotation.y)) * speed;
    }
    else
    {
        position.x -= sin(glm::radians(rotation.y)) * speed;
        position.z -= cos(glm::radians(rotation.y)) * speed;
    }
}
