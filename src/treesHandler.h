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

struct GLDrawArraysIndirectCommand {
	GLuint count;          // Number of vertices to draw
	GLuint instanceCount;  // Number of instances to draw
	GLuint first;          // Offset in the vertex array
	GLuint baseInstance;   // Instance ID
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
	std::vector<glm::vec4> instanceData;

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
	void AddTree(glm::vec3 dir, float height);
	void CreateTextures();
};