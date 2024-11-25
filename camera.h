#pragma once

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
	glm::vec3 position;
	glm::mat4 m_ProjView;

	Camera(GLFWwindow* window, int width, int height);

	void rotate(glm::vec2 mouseDelta);
	void update();

private:
	GLFWwindow* window;

	float aspectRatio;
	float fov;
	float zNear;
	float zFar;

	float speed;

	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 forward;

	float pitch;
	float yaw;
};