#include"Camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
	ClampPosition(); // Ensure initial position is within bounds
}

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform)
{
	// Initializes matrices since otherwise they will be the null matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// Makes camera look in the right direction from the right position
	view = glm::lookAt(Position, Position + Orientation, Up);
	// Adds perspective to the scene
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

	// Exports the camera matrix to the Vertex Shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

void Camera::Inputs(GLFWwindow* window, float deltaTime)
{
	float velocity = speed * deltaTime;

	// Create horizontal-only forward/backward direction (remove Y component)
	glm::vec3 horizontalForward = glm::normalize(glm::vec3(Orientation.x, 0.0f, Orientation.z));
	// Create horizontal-only right direction
	glm::vec3 horizontalRight = glm::normalize(glm::cross(horizontalForward, Up));

	// Handles key inputs with horizontal-only movement and deltaTime
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += velocity * horizontalForward;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position += velocity * -horizontalRight;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position += velocity * -horizontalForward;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += velocity * horizontalRight;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Position += velocity * Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Position += velocity * -Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 10.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 5.0f;
	}

	// Clamp position after movement
	ClampPosition();

	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        // Prevents camera from jumping on the first click
        if (firstClick)
        {
            glfwSetCursorPos(window, (width / 2), (height / 2));
            firstClick = false;
        }

        // Stores the coordinates of the cursor
        double mouseX;
        double mouseY;
        // Fetches the coordinates of the cursor
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
        // and then "transforms" them into degrees 
        float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
        float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

        // Calculates upcoming vertical change in the Orientation
        glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

        // Decides whether or not the next vertical Orientation is legal or not
        if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
        {
            Orientation = newOrientation;
        }

        // Rotates the Orientation left and right
        Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

        // Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
        glfwSetCursorPos(window, (width / 2), (height / 2));
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        // Unhides cursor since camera is not looking around anymore
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // Makes sure the next time the camera looks around it doesn't jump
        firstClick = true;
    }
}

void Camera::ClampPosition()
{
	// Clamp to world boundaries
	Position.x = glm::clamp(Position.x, minBounds.x, maxBounds.x);
	Position.y = glm::clamp(Position.y, minBounds.y, maxBounds.y);
	Position.z = glm::clamp(Position.z, minBounds.z, maxBounds.z);

	// Check collision with billiards table (cylindrical collision)
	glm::vec2 horizontalPos = glm::vec2(Position.x, Position.z);
	glm::vec2 horizontalTableCenter = glm::vec2(tableCenter.x, tableCenter.z);

	float distanceFromTableCenter = glm::length(horizontalPos - horizontalTableCenter);

	// If camera is too close to table center, handle collision smoothly
	if (distanceFromTableCenter < tableRadius)
	{
		// Calculate direction away from table center
		glm::vec2 direction = glm::normalize(horizontalPos - horizontalTableCenter);

		// If the camera is exactly at the center, push it in a default direction
		if (glm::length(direction) < 0.001f)
		{
			direction = glm::vec2(1.0f, 0.0f);
		}

		// Push camera to minimum distance from table
		glm::vec2 newHorizontalPos = horizontalTableCenter + direction * tableRadius;
		Position.x = newHorizontalPos.x;
		Position.z = newHorizontalPos.y;

		// Smooth height adjustment based on distance from table edge
		float edgeDistance = tableRadius - distanceFromTableCenter;
		float heightInfluence = glm::clamp(edgeDistance / tableRadius, 0.0f, 1.0f);

		// Calculate minimum height based on how close we are to the table center
		float minHeightAtThisDistance = tableHeight + 0.5f + (heightInfluence * 1.5f);

		// Only adjust height if we're below the calculated minimum
		if (Position.y < minHeightAtThisDistance)
		{
			// Smoothly interpolate to the required height instead of jumping
			float heightDifference = minHeightAtThisDistance - Position.y;
			Position.y += heightDifference * 0.1f; // Gradual adjustment factor
		}
	}
}

void Camera::SetBounds(glm::vec3 newMinBounds, glm::vec3 newMaxBounds)
{
	minBounds = newMinBounds;
	maxBounds = newMaxBounds;
	ClampPosition(); // Re-clamp with new bounds
}

void Camera::SetTableCollision(glm::vec3 center, float radius, float height)
{
	tableCenter = center;
	tableRadius = radius;
	tableHeight = height;
	ClampPosition(); // Re-clamp with new table parameters
}