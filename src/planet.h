#pragma once

#include "app.h"
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "shader.h"
#include "object.h"
#include "treesHandler.h"

class App;
class Noise;
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
	void DispatchTreeCulling();
	void Draw() override;
	void DebugDraw() override;
	int cubemapResolution;
	float planetScale, noiseAmplitude;
	GLuint noiseCubemapTexture, normalCubemapTexture;
	int seed;

private:
	App* app;
	Noise* noiseGen;
	TreesHandler* treesHandler;

	double lastDispatchTime;
	bool needToDispatch;

	GLuint noise_seedLocation;
	GLuint noiseCubemapLocation, normalCubemapLocation, planetScaleLocation, noiseAmplitudeLocation, terrainLevelsLocation;
	glm::vec3 terrainLevels;

	void LoadPlanet(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices);
};