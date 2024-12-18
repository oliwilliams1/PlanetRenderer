#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "planet.h"

class Planet;

class TreesHandler {
public:
	TreesHandler(Planet* planet);
	~TreesHandler();
	void Draw();

private:
	Planet* planet;
	Shader* shader;
	GLuint computeShaderProgram;
	std::vector<glm::vec3> vertices;

	void FibonacciSphereSampling(int numPoints);

	GLuint VAO, VBO, inputBuffer, outputBuffer;
	void SetupBuffers();
};