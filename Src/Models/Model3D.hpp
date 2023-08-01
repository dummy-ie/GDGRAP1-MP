#ifndef MODEL_3D_HPP
#define MODEL_3D_HPP

namespace model
{
    using namespace glm;
    class Model3D
    {
    private: // model 3d data
        GLuint VAO;
        GLuint VBO;
        GLuint texture;
        GLuint normalTexture;
        int attributesSize;
        std::vector<GLfloat> vertexData;

    public: // model state info
        vec3 position = vec3(0.f);
        vec3 rotation = vec3(0.f);
        vec3 scale = vec3(1.f);
        vec3 color = vec3(1.f);
        // float theta_mod1 = 0;
        // float theta_mod2 = 0;
        // vec4 rgba_mod = vec4(1.0f, 0.72f, 0.77f, 1.0f);

    public:
        Model3D(std::string modelPath, std::string texturePath = "", std::string normalPath = "", vec3 color = vec3(1.f), vec3 pos = vec3(0.f), vec3 rot = vec3(0.f), vec3 sca = vec3(1.f));
        // Model3D(std::string modelPath, vec3 rgba = vec3(1.f), vec3 pos = vec3(0.f), vec3 rot = vec3(0.f), vec3 sca = vec3(1.f));
        ~Model3D();

        void draw(GLuint &shaderProgram);
    };
} // namespace model

#endif // !MODEL_3D_HPP