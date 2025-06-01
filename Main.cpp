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

const unsigned int width = 800;
const unsigned int height = 800;

GLfloat vertices[] = {
	// Wierzcho³ki          /  Kolory     /  TexCoord (u, v) //
	// Przód
	-0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // 0
	 0.5f, -0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 1
	 0.5f,  0.5f,  0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // 2
	-0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // 3

	// Ty³
	-0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // 4
	 0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 5
	 0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // 6
	-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // 7

	// Góra
	-0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // 8
	 0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 9
	 0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   // 10
	-0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // 11

	// Dó³
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
	0, 1, 2, 2, 3, 0,		// Przód
	4, 5, 6, 6, 7, 4,		// Ty³
	8, 9, 10, 10, 11, 8,	// Góra
	12, 13, 14, 14, 15, 12,	// Dó³
	16, 17, 18, 18, 19, 16,	// Lewa
	20, 21, 22, 22, 23, 20	// Prawa
};

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "JakubSputoOpenGL", NULL, NULL);

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

    Camera camera(width, height, glm::vec3(0.0f, 0.0f, 3.0f));

    // Wczytanie modelu
    std::string parentDir = fs::current_path().string();
    std::string modelPath = parentDir + "/models/bilard.glb";
    Model bilardModel(modelPath);

    bool playAnimation = false;

    double prevTime = glfwGetTime();
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

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderProgram.Activate();
        shaderProgram.setInt("texture_diffuse1", 0); // 0 = GL_TEXTURE0

        double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - prevTime);
        prevTime = currentTime;

        camera.Inputs(window, deltaTime);
        camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");

        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        {
            playAnimation = true;
        }
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE)
        {
            playAnimation = false;
        }

        if (playAnimation)
            bilardModel.UpdateAnimation(deltaTime);
        else
            bilardModel.UpdateAnimation(0.0f);

        bilardModel.Draw(shaderProgram);

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
