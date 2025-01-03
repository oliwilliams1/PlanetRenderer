#pragma once

#include "app.h"
#include <string>
#include <glm/gtc/matrix_transform.hpp>

// Must be aligned to 16 bytes, or it will NOT work
struct CameraData {
	glm::mat4 m_ProjView;        // 64 bytes
	glm::mat4 m_CameraRotation;  // 64 bytes
	glm::vec3 position;          // 12 bytes
	float deltaTime;             // 4 bytes
};

struct CamInitData {
	glm::vec3 position;
	float yaw;
	float pitch;
	float fov;
	float speed;
};

class App;

class Camera {
public:
	GLuint UBO;
	glm::vec3 position;
	CameraData cameraData;

	Camera(GLFWwindow* window, float* deltaTime, int width, int height, CamInitData initData, GLuint InputUBO = 0);
	~Camera();
	
	void UpdateBuffers();
	void rotate(glm::dvec2 mouseDelta);
	void update(glm::dvec2 mouseDelta);
	void DebugDraw();

private:
	GLFWwindow* window;
	float* deltaTime;

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

	bool vSync;
};