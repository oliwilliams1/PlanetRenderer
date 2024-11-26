#include "camera.h"

Camera::Camera(GLFWwindow* window, int width, int height) {
	this->window = window;
	this->aspectRatio = (float)width / (float)height;
	this->fov = 70.0f;
	this->zNear = 0.01f;
	this->zFar = 1000.0f;

	this->yaw = -90.0f;
	this->pitch = 0.0f;
	this->speed = 0.05f;
	this->sensitivity = 0.2f;

	this->position = glm::vec3(0, 0, 4.0f);

	this->up = glm::vec3(0, 1, 0);
	this->right = glm::vec3(1, 0, 0);
	this->forward = glm::vec3(0, 0, -1);

    // Init Camera UBO
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	GLuint bindingPoint = 0;
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, UBO);

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

void Camera::update(glm::dvec2 mouseDelta)
{
    // Only rotate if RMB is down
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        this->rotate(mouseDelta);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    // Movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) position += speed * forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) position -= speed * forward;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) position -= glm::normalize(glm::cross(forward, up)) * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) position += glm::normalize(glm::cross(forward, up)) * speed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) position += up * speed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) position -= up * speed;

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

    cameraData.m_ProjView = m_Proj * m_View;

    // Send off to GPU via UBO
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraData), &cameraData);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::debugDraw() const
{
	ImGui::Begin("Camera data view");
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", position.x, position.y, position.z);
	ImGui::Text("Forward: (%.2f, %.2f, %.2f)", forward.x, forward.y, forward.z);
	ImGui::Text("Up: (%.2f, %.2f, %.2f)", up.x, up.y, up.z);
    ImGui::Text("Pitch: %.0fdeg", pitch);
    ImGui::Text("Yaw: %.0fdeg", yaw);
	ImGui::End();
}

Camera::~Camera() {
	glDeleteBuffers(1, &UBO);
}