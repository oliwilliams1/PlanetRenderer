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
	void SetMesh(const std::string& objName);
	virtual void Draw();
	virtual void DebugDraw();

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	void UpdateModelMatrix() const;

	GLuint VAO, VBO, NBO, UVBO, IBO, modelMatrixLocation;
	GLuint albedo, albedoLocation;
	unsigned int indicesCount = 0;

	Shader* shader;

protected:
	void GetModelMatrixLocation();
};