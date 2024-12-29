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

class App;

class Camera {
public:
	GLuint UBO;
	glm::vec3 position;
	CameraData cameraData;

	Camera(App* app);
	~Camera();
	
	void rotate(glm::dvec2 mouseDelta);
	void update(glm::dvec2 mouseDelta);
	void DebugDraw();

private:
	App* app;

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

	// Frame counter + delta time calc
	int currentFPS;
	int frameCount;
	float deltaTime;
	float lastTime;
	float lastDeltaTime;

	bool vSync;
};