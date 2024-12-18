#pragma once

#include "app.h"
#include <glm/glm.hpp>
#include "shader.h"
#include "object.h"
#include "treesHandler.h"

class App;
class TreesHandler;

class PlanetShader : public Shader {
public:
	PlanetShader(const char* vsSource, const char* fsSource, const char* shaderName);

private:
	void AddTesselationShaders();
};

class Planet : public Object {
public:
	Planet(App* app, Shader* shader);
	void Draw() override;
	void DebugDraw() override;
	int cubemapResolution;
	float planetScale, noiseAmplitude;
	GLuint noiseCubemapTexture, normalCubemapTexture;

private:
	App* app;
	TreesHandler* treesHandler;
	GLuint noiseCubemapLocation, normalCubemapLocation, planetScaleLocation, noiseAmplitudeLocation, terrainLevelsLocation;
	glm::vec3 terrainLevels;

	void GeneratePlanet(std::vector<glm::vec3>& vertices);
};