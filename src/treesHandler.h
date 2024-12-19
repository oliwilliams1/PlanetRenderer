#pragma once

#include <vector>
#include <algorithm>
#include <iostream>
#include "planet.h"
#include "cubemap.h"

class Planet;

class TreesHandler {
public:
	TreesHandler(Planet* planet);
	~TreesHandler();

	void Draw();

private:
	Planet* planet;
	Shader* shader;
	Cubemap* noiseCubemapCPU;
	std::vector<glm::vec3> vertices;

	GLuint VAO, VBO;

	void SetupBuffers();
	void FibonacciSphere(int numPoints);
};