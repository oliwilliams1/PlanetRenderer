#pragma once

#include <vector>
#include <glm/common.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Object {
public:
	Object();
	void Draw() const;

private:
	GLuint VAO, VBO;
	GLuint ShaderProgram;
};