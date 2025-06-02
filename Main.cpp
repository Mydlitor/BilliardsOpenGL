#include <iostream>
#include <filesystem>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "tiny_gltf.h"

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"
#include "Model.h"
#include "Skybox.h"

namespace fs = std::filesystem;

const unsigned int width = 1920;
const unsigned int height = 1080;

const int ANIMATION_KEY = GLFW_KEY_T;
const int FILTER_KEY = GLFW_KEY_F;
const int RAINBOW_LIGHT_KEY = GLFW_KEY_R;
const int EXIT_KEY = GLFW_KEY_ESCAPE;

const glm::vec3 CAMERA_START_POSITION(-3.0f, 2.0f, -1.5f);	// Starting position of the camera
const glm::vec3 MIN_BOUNDS(-3.0f, 2.0f, -3.0f);			// Minimum XYZ boundaries
const glm::vec3 MAX_BOUNDS(3.0f, 3.0f, 3.0f);				// Maximum XYZ boundaries
const float DIST_FROM_TABLE = 2.0f;							// Distance from the table center

bool grayscaleFilter = false;
bool rainbowLightFilter = false;

//global pointer so we can access the model from the key callback
Model* g_bilardModel = nullptr;

GLfloat vertices[] = {
	// Wierzcho�ki          /  Kolory     /  TexCoord (u, v) //
	// Prz�d
	-0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // 0
	 0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 1
	 0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // 2
	-0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // 3

	// Ty�
	-0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // 4
	 0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 5
	 0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // 6
	-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // 7

	// G�ra
	-0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // 8
	 0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 9
	 0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // 10
	-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // 11

	// D�
	-0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // 12
	 0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 13
	 0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // 14
	-0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // 15

	// Lewa
	-0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // 16
	-0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 17
	-0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // 18
	-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // 19

	// Prawa
	 0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,    0.0f, 0.0f,   // 20
	 0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,    1.0f, 0.0f,   // 21
	 0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,    1.0f, 1.0f,   // 22
	 0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,    0.0f, 1.0f    // 23
};

GLuint indices[] = {
	0, 1, 2, 2, 3, 0,		// Prz�d
	4, 5, 6, 6, 7, 4,		// Ty�
	8, 9, 10, 10, 11, 8,	// G�ra
	12, 13, 14, 14, 15, 12,	// D�
	16, 17, 18, 18, 19, 16,	// Lewa
	20, 21, 22, 22, 23, 20	// Prawa
};

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == FILTER_KEY && action == GLFW_PRESS)
	{
		grayscaleFilter = !grayscaleFilter;
	}
    if (key == RAINBOW_LIGHT_KEY && action == GLFW_PRESS)
    {
        rainbowLightFilter = !rainbowLightFilter;
    }
	if (key == EXIT_KEY && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	if (key == ANIMATION_KEY && action == GLFW_PRESS)
	{
		if (g_bilardModel != nullptr)
		{
			g_bilardModel->TriggerOneShotAnimation();
		}
	}
}


int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    GLFWwindow* window = glfwCreateWindow(width, height, "JakubSputoOpenGL", monitor, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, width, height);

    Shader shaderProgram("default.vert", "default.frag");
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

	//camera
    Camera camera(width, height, CAMERA_START_POSITION);

	camera.SetBounds(MIN_BOUNDS, MAX_BOUNDS);
	camera.SetTableCollision(glm::vec3(0.0f, 0.0f, 0.0f), DIST_FROM_TABLE, 1.0f);     // Wczytanie modelu
    std::string parentDir = fs::current_path().string();
    std::string modelPath = parentDir + "/models/bilard.glb";
    Model bilardModel(modelPath);    // Wczytanie modelu lampy z transformacją (pozycja nad stolem)
	g_bilardModel = &bilardModel; 
    std::string lampPath = parentDir + "/models/lamp.glb";
    glm::mat4 lampTransform = glm::mat4(1.0f);
    lampTransform = glm::translate(lampTransform, glm::vec3(0.0f, 6.0f, 0.0f)); // Pozycja nad stolem bilardowym
    lampTransform = glm::scale(lampTransform, glm::vec3(0.6f, 0.6f, 0.6f)); // Zmniejszenie rozmiaru lampy
    Model lampModel(lampPath, lampTransform);
    lampModel.SetDoubleSided(true); // Wyłączenie face culling dla lepszej widoczności od wewnątrz

    bool playAnimation = false;

    // 60 FPS limiting
    double prevTime = glfwGetTime();
    double lastFrameTime = glfwGetTime();
    const double targetFPS = 60.0;
    const double targetFrameTime = 1.0 / targetFPS;
    
    float rotation = 1.0f;

	//skybox
	std::vector<std::string> skyboxFaces = {
	"textures/skybox/right.png",
	"textures/skybox/left.png",
	"textures/skybox/top.png",
	"textures/skybox/bottom.png",
	"textures/skybox/front.png",
	"textures/skybox/back.png"
	};

	Skybox skybox(skyboxFaces);

    glm::vec3 baseColor(0.2, 0.8, 0.2);
    glm::vec3 lightPos(0.0, 6.0, 0.0);
    glm::vec3 lightColor(1.0, 1.0, 1.0);

    glfwSetKeyCallback(window, keyCallback);

	while (!glfwWindowShouldClose(window))
	{
        // 60 FPS frame rate limiting
        double currentFrameTime = glfwGetTime();
        double frameTimeDelta = currentFrameTime - lastFrameTime;
        
        if (frameTimeDelta < targetFrameTime) {
            // Sleep for the remaining time to maintain 60 FPS
            double sleepTime = targetFrameTime - frameTimeDelta;
            glfwWaitEventsTimeout(sleepTime);
            currentFrameTime = glfwGetTime();
        }
        lastFrameTime = currentFrameTime;
        
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.Activate();
        shaderProgram.setInt("texture_diffuse1", 0); // 0 = GL_TEXTURE0

        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - prevTime);
        prevTime = currentTime;        camera.Inputs(window, deltaTime);
        camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");


        // Lighting settings
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z);
        glUniform1f(glGetUniformLocation(shaderProgram.ID, "time"), currentTime);
        glUniform1i(glGetUniformLocation(shaderProgram.ID, "enableRainbowLight"), rainbowLightFilter ? 1 : 0);

		skybox.skyboxShader->SetRainbowLight(rainbowLightFilter, currentTime);

		//turn on/off grayscale filter for both shaders
		skybox.skyboxShader->SetGrayscale(grayscaleFilter);
		shaderProgram.SetGrayscale(grayscaleFilter);


        static double lastDebugTime = 0.0;
        if (currentTime - lastDebugTime >= 1.0)
            lastDebugTime = currentTime; // Update animation based on playAnimation state or one-shot animation
        if (playAnimation || bilardModel.IsAnimationPlaying())
            bilardModel.UpdateAnimation(deltaTime);
        else
            bilardModel.UpdateAnimation(0.0f);

        bilardModel.Draw(shaderProgram);
        lampModel.Draw(shaderProgram);

		skybox.Draw(camera, width, height);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shaderProgram.Delete();
	skybox.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
