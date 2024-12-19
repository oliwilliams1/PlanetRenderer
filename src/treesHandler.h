#pragma once

#include <vector>
#include <algorithm>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "planet.h"

class Planet;

class TreesHandler {
public:
	TreesHandler(Planet* planet);
	void Draw();

private:
	Planet* planet;
	Shader* shader;
	std::vector<glm::vec3> vertices;

	unsigned char* posx;
	unsigned char* negx;
	unsigned char* posy;
	unsigned char* negy;
	unsigned char* posz;
	unsigned char* negz;

	unsigned char* SampleCubemap(const glm::vec3& dir);

	GLuint VAO, VBO;

	void SetupBuffers();

	void FibonacciSphere(int numPoints);
};