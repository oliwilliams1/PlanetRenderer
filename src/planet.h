#pragma once

#include <glm/glm.hpp>
#include "object.h"
#include "shader.h"
#include "noise.h"

class PlanetShader : public Shader {
public:
	PlanetShader(const char* vsSource, const char* fsSource, const char* shaderName);

private:
	void AddTesselationShaders();
};

class Planet : public Object {
public:
	Planet(Shader* shader, GLuint noiseTexture);
	void Draw() override;

private:
	GLuint noiseTexture, noiseTextureLocation;
	void GeneratePlanet(std::vector<glm::vec3>& vertices);
};