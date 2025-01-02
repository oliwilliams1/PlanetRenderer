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

// Must be aligned to 16 bytes
struct Tree {
	glm::vec4 pos;           // 16 bytes
	unsigned int treeRot;    // 4 bytes ]
	unsigned int treeType;   // 4 bytes ] -> 8 bytes
	unsigned int padding[2]; // 8 bytes
};

class TreesHandler {
public:
	TreesHandler(Planet* planet);
	~TreesHandler();

	void Draw();
	void UpdateTrees();
	void DebugDraw();
	void CullTrees();

	TreeTextures texturesTree0;

private:
	Planet* planet;
	Shader* shader;
	Cubemap* noiseCubemapCPU;
	std::vector<Tree> instanceData;

	GLuint cullingComputeShader;
	GLuint TreeInputBuffer, TreeOutputBuffer, AtomicCounterBuffer, indirectDrawBuffer;
	GLuint visibleTrees;
	GLuint VAO, VBO;
	int passCounter;
	int numSubdivisions;

	GLuint albedoLocation, normalLocation, treeScaleLocation;
	float treeScale;

	void SetupBuffers();
	void PlaceTrees(int numPoints);
	void PlaceTreesOnTriangle(int numSubdivisons, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3);
	void AddTree(glm::vec3 dir, float height, int randRotation);
	void CreateTextures();
};