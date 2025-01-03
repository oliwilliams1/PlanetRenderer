#include "camera.h"

Camera::Camera(GLFWwindow* window, float* deltaTime, int width, int height, GLuint InputUBO) {
	this->window = window;
	this->deltaTime = deltaTime;
	aspectRatio = (float)width / (float)height;
	fov   = 70.0f;
	zNear = 0.01f;
	zFar  = 10000.0f;

	pitch = -30.0f;
	yaw   = -145.0;
	speed = 500.0f;
	sensitivity = 0.2f;

	position = glm::vec3(400.0f, 1000.0f, 450.0f);

	up = glm::vec3(0, 1, 0);
	right = glm::vec3(1, 0, 0);
	forward = glm::vec3(0, 0, -1);

	vSync = true;

	if (InputUBO == 0) {
		// Init Camera UBO
		glGenBuffers(1, &UBO);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraData), nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		GLuint bindingPoint = 0;
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, UBO);
	}
	else {
		UBO = InputUBO;
	}

	// Init m_ProjView
	update(glm::dvec2(0, 0));
}

void Camera::rotate(glm::dvec2 mouseDelta) {
	yaw += mouseDelta.x * sensitivity;
	pitch -= mouseDelta.y * sensitivity;

	// Clamp values to [-89, 89]
	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;
}

void Camera::UpdateBuffers() {
	// Send off to GPU via UBO
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraData), &cameraData);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::update(glm::dvec2 mouseDelta) {
	// Only rotate if RMB is down
	if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)) {
		this->rotate(mouseDelta);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) speed = 10.0f;
	else speed = 500.0f;

	// Movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) position += *deltaTime * speed * forward;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) position -= *deltaTime * speed * forward;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) position += *deltaTime * glm::normalize(glm::cross(forward, up)) * speed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) position -= *deltaTime * glm::normalize(glm::cross(forward, up)) * speed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) position += *deltaTime * up * speed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) position -= *deltaTime * up * speed;

	// Convert pitch and yaw to radians
	float radYaw = glm::radians(yaw);
	float radPitch = glm::radians(pitch);

	// Calculate a forward vector
	float cosPitch = glm::cos(radPitch);
	float sinPitch = glm::sin(radPitch);
	float cosYaw = glm::cos(radYaw);
	float sinYaw = glm::sin(radYaw);

	forward = glm::vec3(cosYaw * cosPitch, sinPitch, sinYaw * cosPitch);
	forward = glm::normalize(forward);

	// Calculate right up vectors
	right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));
	up = glm::normalize(glm::cross(right, forward));

	// Construct matricies
	glm::mat4 m_View = glm::lookAt(position, position + forward, up);
	glm::mat4 m_Proj = glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar);

	// Construct camera rotation matrix
	glm::mat4 m_CameraRotation = glm::mat4(1.0f);
	m_CameraRotation[0] = glm::vec4(right, 0.0f);
	m_CameraRotation[1] = glm::vec4(up, 0.0f);
	m_CameraRotation[2] = glm::vec4(forward, 0.0f);
	m_CameraRotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	cameraData.m_ProjView       = m_Proj * m_View;
	cameraData.m_CameraRotation = m_CameraRotation;
	cameraData.position         = this->position;
	cameraData.deltaTime        = *deltaTime;

	UpdateBuffers();
}

void Camera::DebugDraw() {
	ImGui::Checkbox("V-Sync", &vSync);
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
	ImGui::Text("Forward: (%.2f, %.2f, %.2f)", forward.x, forward.y, forward.z);
	ImGui::Text("Up: (%.2f, %.2f, %.2f)", up.x, up.y, up.z);
	ImGui::Text("Pitch: %.0fdeg", pitch);
	ImGui::Text("Yaw: %.0fdeg", yaw);

	if (vSync) {
		glfwSwapInterval(1);
	}
	else {
		glfwSwapInterval(0);
	}
}

Camera::~Camera() {
	glDeleteBuffers(1, &UBO);
}