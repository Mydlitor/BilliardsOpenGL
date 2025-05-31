#include <iostream>
#include <filesystem>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"
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

	//Texture
	std::string parentDir = fs::current_path().string();
	std::string texPath = "/textures/";

	Texture texWood((parentDir + texPath + "woodTex.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	Texture texDirt((parentDir + texPath + "dirtTex.jpg").c_str(), GL_TEXTURE_2D, GL_TEXTURE1, GL_RGBA, GL_UNSIGNED_BYTE);
	Texture texBrick((parentDir + texPath + "brickTex.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE2, GL_RGBA, GL_UNSIGNED_BYTE);

	texWood.texUnit(shaderProgram, "tex0", 0);
	texDirt.texUnit(shaderProgram, "tex0", 1);
	texBrick.texUnit(shaderProgram, "tex0", 2);

	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(vertices, sizeof(vertices));

	EBO EBO1(indices, sizeof(indices));

	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();
	
	glEnable(GL_DEPTH_TEST);

	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 3.0f));

	//models
	//creating models
	glm::mat4 model1 = glm::mat4(1.0f);
	glm::mat4 model2 = glm::mat4(1.0f);
	glm::mat4 model3 = glm::mat4(1.0f);

	//starting positions of models
	model1 = glm::translate(model1, glm::vec3(-1.5f, 0.0f, 0.0f)); //move -1.5 to the left
	model2 = glm::translate(model2, glm::vec3(1.5f, 0.0f, 0.0f)); //move 1.5 to the right
	//model3 = glm::translate(model3, glm::vec3(0.0f, 0.0f, 0.0f)); //stay in the middle

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

		camera.Inputs(window);

		camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");

		//for animations
		double currentTime = glfwGetTime();
		if (currentTime - prevTime >= 1.0 / 60)
			prevTime = currentTime;

		VAO1.Bind();

		texWood.Bind();
		model1 = glm::rotate(model1, glm::radians(rotation * -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(model1));
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

		texDirt.Bind();
		model2 = glm::rotate(model2, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(model2));
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

		texBrick.Bind();
		model3 = glm::rotate(model3, glm::radians(rotation), glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(model3));
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

		skybox.Draw(camera, width, height);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	// Delete all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	texWood.Delete();
	texDirt.Delete();
	shaderProgram.Delete();
	skybox.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}