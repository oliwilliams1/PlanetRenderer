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
	void SetData(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<unsigned int> indices);
	void Draw() const;

protected:
	unsigned int indicesCount = 0;
	GLuint VAO, VBO, NBO, IBO;
};