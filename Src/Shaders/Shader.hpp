#ifndef SHADER_HPP
#define SHADER_HPP

namespace shader {
    class Shader {
    public:
        //where the shader program gets stored
        GLuint shaderProgram;

    public:
        Shader(std::fstream vertSrc, std::fstream fragSrc) {
            //vertex shader file
            std::stringstream vertBuff;
            vertBuff << vertSrc.rdbuf();
            std::string vertS = vertBuff.str();
            const char* v = vertS.c_str();

            //frag shader file
            std::stringstream fragBuff;
            fragBuff << fragSrc.rdbuf();
            std::string fragS = fragBuff.str();
            const char* f = fragS.c_str();

            //vertex shader creation
            GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertShader, 1, &v, NULL);
            glCompileShader(vertShader);

            //frag shader creation
            GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragShader, 1, &f, NULL);
            glCompileShader(fragShader);

            //creation of the shader program
            shaderProgram = glCreateProgram();
            glAttachShader(shaderProgram, vertShader);
            glAttachShader(shaderProgram, fragShader);

            //linking of the shader program
            glLinkProgram(shaderProgram);
        }
    };
}

#endif
