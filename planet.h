#pragma once

#include <glm/glm.hpp>
#include "object.h"
#include "shader.h"
#include <algorithm>

class PlanetShader : public Shader {
public:
	PlanetShader(const char* vsSource, const char* fsSource, const char* shaderName);

private:
	void AddTesselationShaders();
};

class Planet : public Object {
public:
	Planet(Shader* shader);
	void Draw() override;

private:
	void GeneratePlanet(std::vector<glm::vec3>& vertices);
};