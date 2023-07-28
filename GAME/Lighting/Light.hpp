#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <string>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace gd
{
    using namespace glm;
    class Light
    {
    public: // light data
        std::string shaderName;

        // vec3 ambient;
        // vec3 diffuse;
        // vec3 specular;

        float ambientStr;
        float specStr;
        float specPhong;

        vec3 lightColor;
        vec3 ambientColor;

    public: // shader name: name of light struct in shader
        Light(std::string shaderName, float ambientStr, float specStr, float specPhong = 32, vec3 lightColor = vec3(1.f), vec3 ambientColor = vec3(1.f)) : 
            shaderName(shaderName),
            ambientStr(ambientStr), 
            specStr(specStr), 
            specPhong(specPhong), 
            lightColor(lightColor), 
            ambientColor(ambientColor) {};

        // apply uniforms in shaderProgram
        void applyUniforms(GLuint &shaderProgram)
        {            
            unsigned int ambientStrLoc = glGetUniformLocation(shaderProgram, (shaderName + ".ambientStr").c_str());
            glUniform1f(ambientStrLoc, ambientStr);

            unsigned int specStrLoc = glGetUniformLocation(shaderProgram, (shaderName + ".specStr").c_str());
            glUniform1f(specStrLoc, specStr);
            
            unsigned int specPhongLoc = glGetUniformLocation(shaderProgram, (shaderName + ".specPhong").c_str());
            glUniform1f(specPhongLoc, specPhong);

            unsigned int lightColorLoc = glGetUniformLocation(shaderProgram, (shaderName + ".lightColor").c_str());
            glUniform3fv(lightColorLoc, 1, value_ptr(lightColor));

            unsigned int ambientColorLoc = glGetUniformLocation(shaderProgram, (shaderName + ".ambientColor").c_str());
            glUniform3fv(ambientColorLoc, 1, value_ptr(ambientColor));
        }

        // pure virtual for the child light classes to apply special/extra uniforms
        // e.g directional light direction, point light position
        virtual void applyExtraUniforms(GLuint &shaderProgram) = 0;
    };
} // namespace gd

#endif // !LIGHT_HPP