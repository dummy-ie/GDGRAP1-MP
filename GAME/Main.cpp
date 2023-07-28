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

using namespace glm;
using namespace gd;

// Screen dimensions
static float height = 600.f;
static float width = 600.f;

// increment to rotate the models by
static float rotationIncrement = 5.f;

// global pointers for both cameras
static PerspectiveCamera *frontCamera;
static OrthoCamera *topCamera;

// global pointers for both models
static Model3D *mainModel;
static Model3D *lightModel;

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
            // pointLight->lightColor = vec3(1.f);
            // pointLight->ambientColor = vec3(1.f);
            lightModel->color = vec3(1.0f, 0.72f, 0.77f);
            mainModel->color = vec3(1.f);
        }
        else
        {
            pointLight->lightColor = vec3(1.f);
            pointLight->ambientColor = vec3(1.f);
            lightModel->color = vec3(1.f);
            mainModel->color = vec3(1.f);
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
            mainModel->rotation.y -= rotationIncrement;
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
            mainModel->rotation.y += rotationIncrement;
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
            mainModel->rotation.x -= rotationIncrement;
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
            mainModel->rotation.x += rotationIncrement;
        }
    }

    // z axis rotation
    if (key == GLFW_KEY_E && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        if (controlLight)
        {
            lightModel->rotation.z -= rotationIncrement;

            updateLightPosition();
        }
        else
        {
            mainModel->rotation.z -= rotationIncrement;
        }
    }

    if (key == GLFW_KEY_Q && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        if (controlLight)
        {
            lightModel->rotation.z += rotationIncrement;

            updateLightPosition();
        }
        else
        {
            mainModel->rotation.z += rotationIncrement;
        }
    }

    // direction light brightness 
    if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS) && controlLight)
    {
        directionLight->ambientStr += 0.1f;
        directionLight->specStr += 0.1f;
    }

    if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS) && controlLight)
    {
        directionLight->ambientStr -= 0.1f;
        directionLight->specStr -= 0.1f;
    }

    // point light brightness
    if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS) && controlLight)
    {
        pointLight->ambientStr += 0.1f;
        pointLight->specStr += 0.1f;
    }

    if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS) && controlLight)
    {
        pointLight->ambientStr -= 0.1f;
        pointLight->specStr -= 0.1f;
    }
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
            frontCamera->position.x = -(cos(glm::radians(frontCamera->rotation.y)) * sin(glm::radians(frontCamera->rotation.x))) * 10.f;
            frontCamera->position.y = -sin(glm::radians(frontCamera->rotation.y)) * 10.f;
            frontCamera->position.z = -(cos(glm::radians(frontCamera->rotation.y)) * cos(glm::radians(frontCamera->rotation.x))) * 10.f;
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

    GLuint shaderProgram;

    // Voxel Link model from https://skfb.ly/6YJOU
    mainModel = new Model3D("Models/source/VoxelLink.obj", "Models/texture/VoxelLink.png", vec3(1.f), vec3(0.f, -2.f, 0.f), vec3(0.f, 0.f, 0.f), vec3(0.2f));

    // Among Us character model from https://skfb.ly/6XXwV
    lightModel = new Model3D("Models/source/among us.obj", "", vec3(1.f), vec3(0.f, 0.f, -10.f), vec3(0.f, 0.f, 0.f), vec3(0.01f));

    frontCamera = new PerspectiveCamera(60, height, width, vec3(0.f, 2.f, 20.f), vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f));
    topCamera = new OrthoCamera(vec3(0.f, 20.f, 0.f), vec3(0.f, -90.f, 0.f), vec3(0.f, 0.f, 0.f));

    directionLight = new DirectionLight("dirLight", vec3(4, 11, -3), 1.f, 2.f, 32, vec3(0.f, 0.f, 1.f), vec3(0.7f, 0.7f, 1.f));
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
            glfwSetCursorPos(window, height / 2.f, width / 2.f);

        /* Render here */
        glFlush();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // update uniforms for both lights
        directionLight->applyUniforms(shaderProgram);
        directionLight->applyExtraUniforms(shaderProgram);

        pointLight->applyUniforms(shaderProgram);
        pointLight->applyExtraUniforms(shaderProgram);

        if (useFrontCamera) // use perspective projection and view matrix
        {
            unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(frontCamera->generateProjectionMatrix()));

            unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(frontCamera->generateViewMatrix()));
        }
        else // use orthographic projection and view matrix
        {
            unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(topCamera->generateProjectionMatrix()));

            unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(topCamera->generateViewMatrix()));
        }
        
        // Draw
        mainModel->draw(shaderProgram);
        lightModel->draw(shaderProgram);

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
