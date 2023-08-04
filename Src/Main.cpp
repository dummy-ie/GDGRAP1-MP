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

#include "Shaders/Shader.hpp"

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
using namespace shader;

// Screen dimensions
static float height = 600.f;
static float width = 600.f;

// increment to rotate/translate the models by
static float positionIncrement = 1.f;
static float rotationIncrement = 5.f;

// global pointers for all cameras
static PerspectiveCamera *thirdPersonCamera;
static PerspectiveCamera *firstPersonCamera;
static OrthoCamera *topCamera;

// in a perfect world i would have created a model manager
static Player *player;
// static Model3D *lightModel;
static Model3D *plane;

// global pointers for both lights
static PointLight *pointLight;
static DirectionLight *directionLight;

// if we're using the perspective or ortho camera
static bool usePerspectiveCamera = true;
static bool useThirdPersonCamera = true; // if using first or third person

// update the light model's position 
// (where the main model is) and then setting the point light's position to it as well
void updateLightPosition()
{
    // lightModel->rotation = glm::radians(lightModel->rotation);
    // lightModel->position = toMat4(quat(lightModel->rotation)) * vec4(lightModel->position, 1.f);

    glm::vec3 position = player->position;

    pointLight->position = position;
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
        usePerspectiveCamera = true;
        useThirdPersonCamera = !useThirdPersonCamera;
    }

    // ortho cam
    if (key == GLFW_KEY_2 && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        usePerspectiveCamera = !usePerspectiveCamera;
        useThirdPersonCamera = true;
        topCamera->position.x = player->position.x;
        topCamera->position.z = player->position.z;
    }

    // y axis rotation
    if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        if (!usePerspectiveCamera)
        {
            // lightModel->rotation.y -= rotationIncrement;
            topCamera->position.x -= positionIncrement;
            // topCamera->position.z = player->position.z;
            // updateLightPosition();
        }
        else if (usePerspectiveCamera)
        {
            player->rotation.z -= rotationIncrement;
        }
    }

    if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        if (!usePerspectiveCamera)
        {
            topCamera->position.x += positionIncrement;
            // topCamera->position.z = player->position.z;

            // updateLightPosition();
        }
        else if (usePerspectiveCamera)
        {
            player->rotation.z += rotationIncrement;
        }
    }

    // x axis rotation
    if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        if (!usePerspectiveCamera)
        {
            // topCamera->position.x = player->position.x;
            topCamera->position.z -= positionIncrement;

            // updateLightPosition();
        }
        else if (usePerspectiveCamera && useThirdPersonCamera)
        {
            player->directionalMove(true);
            updateLightPosition();
        }
        else if (usePerspectiveCamera && !useThirdPersonCamera && firstPersonCamera->rotation.y <= 90.f)
        {
            firstPersonCamera->rotation.y += rotationIncrement;
        }
    }

    if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        if (!usePerspectiveCamera)
        {
            // topCamera->position.x = player->position.x;
            topCamera->position.z += positionIncrement;

            // updateLightPosition();
        }
        else if (usePerspectiveCamera && useThirdPersonCamera)
        {
            player->directionalMove(false);
            updateLightPosition();
        }
        else if (usePerspectiveCamera && !useThirdPersonCamera && firstPersonCamera->rotation.y >= -90.f)
        {
            firstPersonCamera->rotation.y -= rotationIncrement;
        }
    }

    if (key == GLFW_KEY_E && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        if (usePerspectiveCamera && !useThirdPersonCamera && firstPersonCamera->fov <= 120.f)
            firstPersonCamera->fov += 5.f;
    }

    if (key == GLFW_KEY_Q && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        if (usePerspectiveCamera && !useThirdPersonCamera && firstPersonCamera->fov >= 10.f)
            firstPersonCamera->fov -= 5.f;
    }

    if (key == GLFW_KEY_F && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        if (pointLight->specStr >= 2.f)
        {
            pointLight->ambientStr = 0.7f;
            pointLight->specStr = 0.5f;
        }
        pointLight->ambientStr += 0.7f;
        pointLight->specStr += 0.5f;
    }

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

    // if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
    // {
    // }

    // if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
    // {
    // }
}

static void Cursor_Position_Callback(GLFWwindow *window, double xpos, double ypos)
{
    if (glfwGetWindowAttrib(window, GLFW_FOCUSED))
    {
        if (usePerspectiveCamera)
        {
            // some dark magic ritual that works!
            // rotate the camera using the mouse's offset from the center while we keep the mouse at the center of the screen.
            thirdPersonCamera->rotation.x += 0.1f * float(width / 2 - xpos);
            thirdPersonCamera->rotation.y += 0.1f * float(height / 2 - ypos);
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
    window = glfwCreateWindow(height, width, "Marcus Leocario / Joachim Arguelles", NULL, NULL);
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

    Shader skybox("Shaders/skybox.vert", "Shaders/skybox.frag");

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

    // model from https://free3d.com/3d-model/german-wwii-era-heavy-tank-tiger-i-254401.html
    player = new Player("Models/source/tanknew.obj", "Models/texture/tank.jpg", "Models/texture/tank_normal.jpg", vec3(1.f), vec3(0.f, 0.7f, 0.f), vec3(-90.f, 0.f, 0.f), vec3(1.f));

    // Among Us character model from https://skfb.ly/6XXwV
    // Model3D amogus("Models/source/among us.obj", "Plastic_4K_Diffuse.jpg", "Plastic_4K_Normal.jpg", vec3(1.f), vec3(0.f, 0.f, -30.f), vec3(0.f, 0.f, 0.f), vec3(0.01f));
    
    // model from https://www.turbosquid.com/3d-models/fictional-pbr-tank-3d-model-1382107
    Model3D fictionalTank("Models/source/fictionaltank.obj", "Models/texture/fictionaltank.jpg");
    fictionalTank.position = vec3(40.f, 3.f, -5.f);
    fictionalTank.rotation = vec3(-90.f, 0.f, 32.f);
    
    // model from https://www.turbosquid.com/3d-models/3d-model-of-tank/899695
    Model3D genericTank("Models/source/generictank.obj", "Models/texture/generictank.jpg");
    genericTank.position = vec3(60.f, 3.f, 50.f);
    genericTank.rotation = vec3(-90.f, 0.f, 43.f);
    
    // model from https://www.turbosquid.com/3d-models/free-3ds-mode-wiesel-2-ozelot-anti-air/361920
    Model3D ozelot("Models/source/ozelot.obj", "Models/texture/ozelot.jpg");
    ozelot.position = vec3(10.f, 3.f, 80.f);
    ozelot.rotation = vec3(-90.f, 0.f, 56.f);
    
    // model from https://www.turbosquid.com/3d-models/free-sherman-3d-model/949824
    Model3D sherman("Models/source/sherman.obj", "Models/texture/sherman.jpg");
    sherman.position = vec3(-30.f, 3.f, 30.f);
    sherman.rotation = vec3(-90.f, 0.f, 47.f);
    
    // modified model, original from https://free3d.com/3d-model/t-90a-russian-tank-47395.html
    Model3D t90broken("Models/source/t90broken.obj", "Models/texture/t90broken.png");
    t90broken.position = vec3(-30.f, 1.f, 0.f);
    t90broken.rotation = vec3(-90.f, 0.f, 72.f);

    // model from https://skfb.ly/orGPV
    Model3D deadTree("Models/source/DeadTree_LoPoly.obj", "Models/texture/DeadTree_LoPoly_DeadTree_Diffuse.jpg", "Models/texture/DeadTree_LoPoly_DeadTree_Normal.jpg");
    deadTree.position = vec3(-100.f, 1.f, -30.f);

    // Model3D fictionalTank("Models/source/fictionaltank.obj", "Models/texture/fictionaltank.png");

    plane = new Model3D("Models/source/plane.obj", "Models/texture/Grass.png", "", vec3(1.f), vec3(0.f, 0.f, 0.f), vec3(-90.f, 0.f, 0.f), vec3(1000.f));

    thirdPersonCamera = new PerspectiveCamera(60, height, width, vec3(0.f, 2.f, 20.f), vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f));
    firstPersonCamera = new PerspectiveCamera(60, height, width, vec3(0.f, 2.f, 20.f), vec3(0.f, 0.f, 0.f), vec3(0.f, 0.f, 0.f));
    topCamera = new OrthoCamera(vec3(0.f, 50.f, 0.f), vec3(0.f, -90.f, 0.f), vec3(0.f, 0.f, 0.f));

    directionLight = new DirectionLight("dirLight", vec3(0, -10, 5), 0.1f, 0.2f, 32, vec3(0.3f, 0.3f, 1.f), vec3(0.3f, 0.3f, 1.f));
    pointLight = new PointLight("pointLight", vec3(0.f, 3.f, 0.f), 0.5f, 0.7f, 32, vec3(1.f, 1.f, 1.f), vec3(1.f, 1.f, 1.f));

    std::cout << "loaded cameras" << std::endl;

    Shader sample("Shaders/sample.vert", "Shaders/sample.frag");

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // if using perspective cam, lock the mouse to the center to use the first person cam controls
        if (usePerspectiveCamera)
        {
            if (useThirdPersonCamera)
            {
                glfwSetCursorPos(window, height / 2.f, width / 2.f);

                // translate the camera 10 units away from the player in all directions and negate its (direction) rotation to keep it aimed at the player
                thirdPersonCamera->position.x = player->position.x - (cos(glm::radians(thirdPersonCamera->rotation.y)) * sin(glm::radians(thirdPersonCamera->rotation.x))) * 10.f;
                thirdPersonCamera->position.y = (player->position.y + 3.f) - sin(glm::radians(thirdPersonCamera->rotation.y)) * 10.f;
                thirdPersonCamera->position.z = player->position.z - (cos(glm::radians(thirdPersonCamera->rotation.y)) * cos(glm::radians(thirdPersonCamera->rotation.x))) * 10.f;

                // limit the camera so u can't go through the ground
                if (thirdPersonCamera->position.y <= 0.1f)
                    thirdPersonCamera->position.y = 0.1f;
            }
        }

        firstPersonCamera->rotation.x = player->rotation.z + 180.f;
        firstPersonCamera->position.x = player->position.x - sin(glm::radians(player->rotation.z)) * 5.f;
        firstPersonCamera->position.z = player->position.z - cos(glm::radians(player->rotation.z)) * 5.f;

        pointLight->position.x = player->position.x - sin(glm::radians(player->rotation.z)) * 5.f;
        pointLight->position.z = player->position.z - cos(glm::radians(player->rotation.z)) * 5.f;

        Camera *currentCamera;

        if (usePerspectiveCamera && useThirdPersonCamera) // use perspective projection and view matrix
            currentCamera = thirdPersonCamera;
        else if (usePerspectiveCamera && !useThirdPersonCamera)
            currentCamera = firstPersonCamera;
        else // use orthographic projection and view matrix
            currentCamera = topCamera;

        /* Render here */
        glFlush();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        glUseProgram(skybox.shaderProgram);

        glm::mat4 skyView = glm::mat4(1.f);
        skyView = glm::mat4(
            glm::mat3(currentCamera->generateViewMatrix()));

        unsigned int sky_ViewLoc = glGetUniformLocation(skybox.shaderProgram, "view");
        glUniformMatrix4fv(sky_ViewLoc, 1, GL_FALSE, glm::value_ptr(skyView));

        unsigned int sky_ProjectionLoc = glGetUniformLocation(skybox.shaderProgram, "projection");
        glUniformMatrix4fv(sky_ProjectionLoc, 1, GL_FALSE, glm::value_ptr(currentCamera->generateProjectionMatrix()));

        unsigned int sky_useThirdPersonCameraLoc = glGetUniformLocation(skybox.shaderProgram, "useThirdPersonCamera");
        glUniform1i(sky_useThirdPersonCameraLoc, useThirdPersonCamera);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glUseProgram(sample.shaderProgram);

        unsigned int cameraPosLoc = glGetUniformLocation(sample.shaderProgram, "cameraPos");
        glUniform3fv(cameraPosLoc, 1, glm::value_ptr(currentCamera->position));

        // update uniforms for both lights
        directionLight->applyUniforms(sample.shaderProgram);
        directionLight->applyExtraUniforms(sample.shaderProgram);

        pointLight->applyUniforms(sample.shaderProgram);
        pointLight->applyExtraUniforms(sample.shaderProgram);

        unsigned int projectionLoc = glGetUniformLocation(sample.shaderProgram, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(currentCamera->generateProjectionMatrix()));

        unsigned int viewLoc = glGetUniformLocation(sample.shaderProgram, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(currentCamera->generateViewMatrix()));

        unsigned int usePerspectiveCameraLoc = glGetUniformLocation(sample.shaderProgram, "usePerspectiveCamera");
        glUniform1i(usePerspectiveCameraLoc, usePerspectiveCamera);

        unsigned int useThirdPersonCameraLoc = glGetUniformLocation(sample.shaderProgram, "useThirdPersonCamera");
        glUniform1i(useThirdPersonCameraLoc, useThirdPersonCamera);

        // Draw
        player->draw(sample.shaderProgram);
        // amogus.draw(sample.shaderProgram);
        fictionalTank.draw(sample.shaderProgram);
        genericTank.draw(sample.shaderProgram);
        ozelot.draw(sample.shaderProgram);
        sherman.draw(sample.shaderProgram);
        t90broken.draw(sample.shaderProgram);
        deadTree.draw(sample.shaderProgram);


        plane->draw(sample.shaderProgram);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
