#include "camera.h"

Camera::Camera(GLFWwindow* window, int width, int height) {
	this->window = window;
	this->aspectRatio = (float)width / (float)height;
	this->fov = 70.0f;
	this->zNear = 0.01f;
	this->zFar = 1000.0f;
	this->yaw = 0.0f;
	this->pitch = 0.0f;
	this->speed = 0.01f;

	this->up = glm::vec3(0, 1, 0);
	this->right = glm::vec3(1, 0, 0);
	this->forward = glm::vec3(0, 0, -1);
}

void Camera::update()
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		position += speed * forward;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= speed * forward;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= glm::normalize(glm::cross(forward, up)) * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		position += glm::normalize(glm::cross(forward, up)) * speed;
	}

	yaw, pitch = glm::radians(yaw), glm::radians(pitch);
	
	float cosPitch = glm::cos(pitch);
	float sinPitch = glm::sin(pitch);

	float cosYaw = glm::cos(yaw);
	float sinYaw = glm::sin(yaw);

	forward = glm::vec3(cosYaw * cosPitch, sinPitch, sinYaw * cosPitch);

	glm::vec3 rightUp = glm::cross(forward, glm::vec3(0, 1, 0));
	right = glm::normalize(rightUp);
	up = glm::normalize(glm::cross(right, forward));
	
	glm::mat4 m_View = glm::lookAt(position, position + forward, up);
	glm::mat4 m_Proj = glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar);

	m_ProjView = m_Proj * m_View;
}