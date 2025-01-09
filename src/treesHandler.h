#pragma once

#include <vector>
#include <algorithm>
#include <iostream>
#include "planet.h"
#include "cubemap.h"
#include <random>

class Planet;

struct TreeTextures {
	GLuint topAlbedo, topNormal;
};

class TreesHandler {
public:
	TreesHandler(Planet* planet);
	~TreesHandler();

	void Draw();
	void UpdateTrees();
	void DebugDraw();

	TreeTextures texturesTree0;

private:
	Planet* planet;
	Shader* imposterShader;
	Cubemap* noiseCubemapCPU;
	std::vector<glm::vec3> instanceData;

	GLuint ImposterVAO, ImposterVBO, ImposterIBO;
	GLuint TreeVAO, TreeVBO, TreeIBO;

	int passCounter;
	int numSubdivisions;

	GLuint albedoLocation, normalLocation, treeScaleLocation;
	float treeScale;

	void SetupBuffers();
	void PlaceTrees(int numPoints);
	void PlaceTreesOnTriangle(int numSubdivisons, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
	void AddTree(glm::vec3 dir, float height);
	void CreateTextures();
};