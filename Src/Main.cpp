// #include "glad.c"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "Extensions/tiny_obj_loader.h"

#include <iostream>
#include <string>

#include "Camera/Camera.cpp"
#include "Camera/OrthoCamera.hpp"
#include "Camera/PerspectiveCamera.hpp"

#include "Lighting/Light.hpp"
#include "Lighting/DirectionLight.hpp"
#include "Lighting/PointLight.hpp"

#include "Models/Model3D.cpp"
#include "Models/Player.cpp"

using namespace glm;
using namespace gd;

// Screen dimensions
static float height = 600.f;
static float width = 600.f;

// increment to rotate the models by
static float positionIncrement = 1.f;
static float rotationIncrement = 5.f;

// global pointers for both cameras
static PerspectiveCamera *frontCamera;
static OrthoCamera *topCamera;

// global pointers for both models
static Player *player;
static Model3D *lightModel;
static Model3D *plane;

// global pointers for both lights
static PointLight *pointLight;
static DirectionLight *directionLight;

// if we're using the perspective or ortho camera
static bool useFrontCamera = true;

// if we're controlling the center model or the light model
static bool controlLight = false;

// update the light model's position by rotating it around the origin
// (where the main model is) and then setting the point light's position to it as well
void updateLightPosition()
{
    lightModel->rotation = glm::radians(lightModel->rotation);
    lightModel->position = toMat4(quat(lightModel->rotation)) * vec4(lightModel->position, 1.f);

    pointLight->position = lightModel->position;
}

static void Key_Callback(
    GLFWwindow *window,
    int key,
    int scancode,
    int action,
    int mod)
{
    // perspective cam
    if (key == GLFW_KEY_1 && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        useFrontCamera = true;
    }

    // ortho cam
    if (key == GLFW_KEY_2 && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        useFrontCamera = false;
    }

    // switch model
    if (key == GLFW_KEY_SPACE && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        controlLight = !controlLight;
        if (controlLight)
        {
            pointLight->lightColor = vec3(1.0f, 0.72f, 0.77f);
            pointLight->ambientColor = vec3(1.0f, 0.72f, 0.77f);
            lightModel->color = vec3(1.0f, 0.72f, 0.77f);
            player->color = vec3(1.f);
        }
        else
        {
            pointLight->lightColor = vec3(1.f);
            pointLight->ambientColor = vec3(1.f);
            lightModel->color = vec3(1.f);
            player->color = vec3(1.f);
        }
    }

    // y axis rotation
    if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        if (controlLight)
        {
            lightModel->rotation.y -= rotationIncrement;

            updateLightPosition();
        }
        else
        {
            player->rotation.y -= rotationIncrement;
        }
    }

    if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        if (controlLight)
        {
            lightModel->rotation.y += rotationIncrement;

            updateLightPosition();
        }
        else
        {
            player->rotation.y += rotationIncrement;
        }
    }

    // x axis rotation
    if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        if (controlLight)
        {
            lightModel->rotation.x -= rotationIncrement;

            updateLightPosition();
        }
        else
        {
            player->directionalMove(true);
        }
    }

    if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        if (controlLight)
        {
            lightModel->rotation.x += rotationIncrement;

            updateLightPosition();
        }
        else
        {
            player->directionalMove(false);
        }
    }

    // z axis rotation
    // if (key == GLFW_KEY_E && (action == GLFW_REPEAT || action == GLFW_PRESS))
    // {
    //     if (controlLight)
    //     {
    //         lightModel->rotation.z -= rotationIncrement;

    //         updateLightPosition();
    //     }
    //     else
    //     {
    //         player->position.z -= positionIncrement;
    //     }
    // }

    // if (key == GLFW_KEY_Q && (action == GLFW_REPEAT || action == GLFW_PRESS))
    // {
    //     if (controlLight)
    //     {
    //         lightModel->rotation.z += rotationIncrement;

    //         updateLightPosition();
    //     }
    //     else
    //     {
    //         player->position.z += positionIncrement;
    //     }
    // }

    // direction light brightness
    // if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS) && controlLight)
    // {
    //     directionLight->ambientStr += 0.1f;
    //     directionLight->specStr += 0.1f;
    // }

    // if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS) && controlLight)
    // {
    //     directionLight->ambientStr -= 0.1f;
    //     directionLight->specStr -= 0.1f;
    // }

    // // point light brightness
    // if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS) && controlLight)
    // {
    //     pointLight->ambientStr += 0.1f;
    //     pointLight->specStr += 0.1f;
    // }

    // if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS) && controlLight)
    // {
    //     pointLight->ambientStr -= 0.1f;
    //     pointLight->specStr -= 0.1f;
    // }
}

static void Cursor_Position_Callback(GLFWwindow *window, double xpos, double ypos)
{
    if (glfwGetWindowAttrib(window, GLFW_FOCUSED))
    {
        if (useFrontCamera)
        {
            // some dark magic ritual that works!
            // rotate the camera using the mouse's offset from the center while we keep the mouse at the center of the screen.
            // translate the camera 10 units away in all directions and negate its (direction) rotation to keep it aimed at the origin (0, 0, 0)
            frontCamera->rotation.x += 0.1f * float(width / 2 - xpos);
            frontCamera->rotation.y += 0.1f * float(height / 2 - ypos);
        }
    }
}

void Mouse_Button_Callback(GLFWwindow *window, int button, int action, int mods)
{
    // double x = 0;
    // double y = 0;

    // click to reset
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // x_mod = 0;
        // y_mod = 0;
        // z_mod = 0;
        // scale_mod = 0.1;
        // fov_mod = 60;
        // scale_x_mod = 0;
        // scale_y_mod = 0;
        // scale_z_mod = 0;
        // theta_hor = 0;
        // theta_ver = 0;
    }
}

int main(void)
{
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(height, width, "Marcus Leocario", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    gladLoadGL();

    glfwSetKeyCallback(window, Key_Callback);
    glfwSetCursorPosCallback(window, Cursor_Position_Callback);
    glfwSetMouseButtonCallback(window, Mouse_Button_Callback);

    std::fstream sky_vertSrc("Shaders/skybox.vert");
    std::stringstream sky_vertBuff;
    sky_vertBuff << sky_vertSrc.rdbuf();
    std::string sky_vertS = sky_vertBuff.str();
    const char *sky_v = sky_vertS.c_str();

    std::fstream sky_fragSrc("Shaders/skybox.frag");
    std::stringstream sky_fragBuff;
    sky_fragBuff << sky_fragSrc.rdbuf();
    std::string sky_fragS = sky_fragBuff.str();
    const char *sky_f = sky_fragS.c_str();

    GLuint sky_vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sky_vertShader, 1, &sky_v, NULL);
    glCompileShader(sky_vertShader);

    GLuint sky_fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sky_fragShader, 1, &sky_f, NULL);
    glCompileShader(sky_fragShader);

    GLuint skyboxProgram = glCreateProgram();
    glAttachShader(skyboxProgram, sky_vertShader);
    glAttachShader(skyboxProgram, sky_fragShader);

    glLinkProgram(skyboxProgram);

    // glDeleteShader(sky_vertShader);
    // glDeleteShader(sky_fragShader);

    /*
  7--------6
 /|       /|
4--------5 |
| |      | |
| 3------|-2
|/       |/
0--------1
*/
    // Vertices for the cube
    float skyboxVertices[]{
        -1.f, -1.f, 1.f,  // 0
        1.f, -1.f, 1.f,   // 1
        1.f, -1.f, -1.f,  // 2
        -1.f, -1.f, -1.f, // 3
        -1.f, 1.f, 1.f,   // 4
        1.f, 1.f, 1.f,    // 5
        1.f, 1.f, -1.f,   // 6
        -1.f, 1.f, -1.f   // 7
    };

    // Skybox Indices
    unsigned int skyboxIndices[]{
        1, 2, 6,
        6, 5, 1,

        0, 4, 7,
        7, 3, 0,

        4, 5, 6,
        6, 7, 4,

        0, 3, 2,
        2, 1, 0,

        0, 1, 5,
        5, 4, 0,

        3, 7, 6,
        6, 2, 3};

    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_INT) * 36, &skyboxIndices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);

    // skybox textures from https://www.pngwing.com/en/free-png-hzcii
    std::string facesSkybox[]{
        "Models/Skybox/Night/posx.jpg",
        "Models/Skybox/Night/negx.jpg",
        "Models/Skybox/Night/posy.jpg",
        "Models/Skybox/Night/negy.jpg",
        "Models/Skybox/Night/posz.jpg",
        "Models/Skybox/Night/negz.jpg",
    };

    unsigned int skyboxTex;
    glGenTextures(1, &skyboxTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; i++)
    {
        int w, h, skyCChannel;
        stbi_set_flip_vertically_on_load(false);

        unsigned char *data = stbi_load(facesSkybox[i].c_str(), &w, &h, &skyCChannel, 0);

        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }

        stbi_image_free(data);
    }

    GLuint shaderProgram;

    // Voxel Link model from https://skfb.ly/6YJOU
    player = new Player("Models/source/VoxelLink.obj", "Models/texture/VoxelLink.png", vec3(1.f), vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(0.2f));

    // Among Us character model from https://skfb.ly/6XXwV
    lightModel = new Model3D("Models/source/among us.obj", "", vec3(1.f), vec3(0.f, 0.f, -10.f), vec3(0.f, 0.f, 0.f), vec3(0.01f));

    plane = new Model3D("Models/source/plane.obj", "Models/texture/Grass.png", vec3(1.f), vec3(0.f, 0.f, 0.f), vec3(-90.f, 0.f, 0.f), vec3(1000.f));

    frontCamera = new PerspectiveCamera(60, height, width, vec3(0.f, 2.f, 20.f), vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f));
    topCamera = new OrthoCamera(vec3(0.f, 20.f, 0.f), vec3(0.f, -90.f, 0.f), vec3(0.f, 0.f, 0.f));

    directionLight = new DirectionLight("dirLight", vec3(0, -10, 5), 0.1f, 0.2f, 32, vec3(0.3f, 0.3f, 1.f), vec3(0.3f, 0.3f, 1.f));
    pointLight = new PointLight("pointLight", lightModel->position + vec3(0.f, 2.f, 0.f), 0.5f, 0.7f, 32, vec3(1.f, 1.f, 1.f), vec3(1.f, 1.f, 1.f));

    std::cout << "loaded cameras" << std::endl;

    std::fstream vertSrc("Shaders/sample.vert");
    std::stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();
    std::string vertS = vertBuff.str();
    const char *v = vertS.c_str();

    std::fstream fragSrc("Shaders/sample.frag");
    std::stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();
    std::string fragS = fragBuff.str();
    const char *f = fragS.c_str();

    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &v, NULL);
    glCompileShader(vertShader);

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &f, NULL);
    glCompileShader(fragShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);

    glLinkProgram(shaderProgram);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // if using perspective cam, lock the mouse to the center to use the first person cam controls
        if (useFrontCamera)
        {
            glfwSetCursorPos(window, height / 2.f, width / 2.f);
            frontCamera->position.x = player->position.x - (cos(glm::radians(frontCamera->rotation.y)) * sin(glm::radians(frontCamera->rotation.x))) * 10.f;
            frontCamera->position.y = (player->position.y + 3.f) - sin(glm::radians(frontCamera->rotation.y)) * 10.f;
            frontCamera->position.z = player->position.z - (cos(glm::radians(frontCamera->rotation.y)) * cos(glm::radians(frontCamera->rotation.x))) * 10.f;
        }

        Camera* currentCamera; 

        if (useFrontCamera) // use perspective projection and view matrix
            currentCamera = frontCamera;
        else // use orthographic projection and view matrix
            currentCamera = topCamera;

        /* Render here */
        glFlush();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        glUseProgram(skyboxProgram);

        glm::mat4 skyView = glm::mat4(1.f);
        skyView = glm::mat4(
            glm::mat3(currentCamera->generateViewMatrix()));

        unsigned int sky_ViewLoc = glGetUniformLocation(skyboxProgram, "view");
        glUniformMatrix4fv(sky_ViewLoc, 1, GL_FALSE, glm::value_ptr(skyView));

        unsigned int sky_ProjectionLoc = glGetUniformLocation(skyboxProgram, "projection");
        glUniformMatrix4fv(sky_ProjectionLoc, 1, GL_FALSE, glm::value_ptr(currentCamera->generateProjectionMatrix()));

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glUseProgram(shaderProgram);

        unsigned int cameraPosLoc = glGetUniformLocation(shaderProgram, "cameraPos");
        glUniform3fv(cameraPosLoc, 1, glm::value_ptr(currentCamera->position));

        // update uniforms for both lights
        directionLight->applyUniforms(shaderProgram);
        directionLight->applyExtraUniforms(shaderProgram);

        pointLight->applyUniforms(shaderProgram);
        pointLight->applyExtraUniforms(shaderProgram);

        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(currentCamera->generateProjectionMatrix()));

        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(currentCamera->generateViewMatrix()));

        // Draw
        player->draw(shaderProgram);
        lightModel->draw(shaderProgram);

        plane->draw(shaderProgram);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
