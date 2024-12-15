#pragma once

#include "app.h"
#include <glm/glm.hpp>
#include "shader.h"
#include "object.h"

class App;

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

private:
	App* app;
	GLuint noiseCubemapTexture, noiseCubemapLocation, normalCubemapLocation, normalCubemapTexture, planetScaleLocation, noiseAmplitudeLocation, terrainLevelsLocation;
	float planetScale, noiseAmplitude;
	glm::vec3 terrainLevels;

	void GeneratePlanet(std::vector<glm::vec3>& vertices);
};