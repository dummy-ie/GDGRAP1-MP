#ifndef DIRECTION_LIGHT_HPP
#define DIRECTION_LIGHT_HPP

#include "Light.hpp"

namespace gd
{
    using namespace glm;
    class DirectionLight : public Light
    {
    public:
        vec3 direction;

    public:
        DirectionLight(std::string shaderName, vec3 direction, float ambientStr, float specStr, float specPhong, vec3 lightColor, vec3 ambientColor) : 
        direction(direction), Light(shaderName, ambientStr, specStr, specPhong, lightColor, ambientColor) {}

        // sending uniform for direction of the light
        void applyExtraUniforms(GLuint &shaderProgram)
        {
            unsigned int directionLoc = glGetUniformLocation(shaderProgram, (shaderName + ".direction").c_str());
            glUniform3fv(directionLoc, 1, value_ptr(direction));
        }
    };
} // namespace gd


#endif // !DIRECTION_LIGHT_HPP