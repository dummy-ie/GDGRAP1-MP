#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Model3D.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

namespace model
{
    using namespace glm;
    class Player : public Model3D
    {
        public:
            float speed = 0.5f;

        public:
            Player(std::string modelPath, std::string texturePath = "", std::string normalPath = "", vec3 color = vec3(1.f), vec3 pos = vec3(0.f), vec3 rot = vec3(0.f), vec3 sca = vec3(1.f));

            void directionalMove(bool isForward); 
    };
} // namespace model

#endif // !PLAYER_HPP