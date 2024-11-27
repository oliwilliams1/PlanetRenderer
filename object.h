#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"

class Object {
public:
	Object(Shader* shader);
	void SetData(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<unsigned int> indices);
	void Draw() const;
	void ObjectDebugImGUI();

protected:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	unsigned int indicesCount = 0;
	GLuint VAO, VBO, NBO, IBO, modelMatrixLocation;
	Shader* shader;

	void GetModelMatrixLocation();
	void UpdateModelMatrix() const;
};