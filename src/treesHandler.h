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

	GLuint treeVAO, treeVBO;
	void SetupBuffers();
};