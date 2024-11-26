#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Must be aligned to 16 bytes, or it will NOT work
struct CameraData {
	glm::mat4 m_ProjView; // 16 bytes
};

class Camera {
public:
	GLuint UBO;
	glm::vec3 position;
	CameraData cameraData;

	Camera(GLFWwindow* window, int width, int height);
	~Camera();
	
	void rotate(glm::dvec2 mouseDelta);
	void update(glm::dvec2 mouseDelta);
	void debugDraw() const;

private:
	GLFWwindow* window;

	float aspectRatio;
	float fov;
	float zNear;
	float zFar;
	float sensitivity;

	float speed;

	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 forward;

	float pitch;
	float yaw;
};