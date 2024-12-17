#pragma once

#include "planet.h"

class Planet;

struct Tree {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

class TreesHandler {
public:
	TreesHandler(Planet* planet);
	~TreesHandler();
	void SetupTreesForPlanet();
	void DrawTrees();

private:
	Planet* planet;
	int cubemapResolution;
	std::vector<float> cubemapData;
	void RetrieveCubemapData();
	glm::vec4 SampleCubemap(const glm::vec3& direction);
};