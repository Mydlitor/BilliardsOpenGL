#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include"shaderClass.h"

class Camera
{
public:
	// Stores the main vectors of the camera
	glm::vec3 Position;
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	// Prevents the camera from jumping around when first clicking left click
	bool firstClick = true;

	// Stores the width and height of the window
	int width;
	int height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float speed = 0.1f;
	float sensitivity = 100.0f;

	// Camera clamping boundaries
	glm::vec3 minBounds = glm::vec3(-10.0f, 0.5f, -10.0f);  // Minimum XYZ boundaries
	glm::vec3 maxBounds = glm::vec3(10.0f, 8.0f, 10.0f);    // Maximum XYZ boundaries

	// Billiards table collision parameters
	glm::vec3 tableCenter = glm::vec3(0.0f, 0.0f, 0.0f);    // Center of billiards table
	float tableRadius = 3.0f;                                // Minimum distance from table center
	float tableHeight = 1.0f;                               // Height of the table surface

	// Camera constructor to set up initial values
	Camera(int width, int height, glm::vec3 position);

	// Updates and exports the camera matrix to the Vertex Shader
	void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform);
	// Handles camera inputs
	void Inputs(GLFWwindow* window, float deltaTime);

	// Camera clamping functions
	void ClampPosition();
	void SetBounds(glm::vec3 minBounds, glm::vec3 maxBounds);
	void SetTableCollision(glm::vec3 center, float radius, float height);
};
#endif