#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "planet.h"

class Planet;

struct Tree {
	glm::mat4 m_Model;
	glm::vec3 pos;

	Tree(glm::mat4 m_Model, glm::vec3 pos) : m_Model(m_Model), pos(pos) {}
};

class TreesHandler {
public:
	TreesHandler(Planet* planet);
	~TreesHandler();
	void Draw();

private:
	Planet* planet;
	Shader* shader;

	std::vector<Tree> trees;

	int cubemapResolution;
	std::vector<float> cubemapData;

	void RetrieveCubemapData();
	void FibonacciSphereSampling(int numPoints);
	glm::vec4 SampleCubemap(const glm::vec3& direction);

	GLuint treeVAO, treeVBO, instanceVBO;
	void SetupBuffers();

	glm::vec3 dir = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
};