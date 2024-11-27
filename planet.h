#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include "object.h"
#include "shader.h"

class Planet : public Object {
public:
    Planet();
private: 
    void GeneratePlanet(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices);
    void SubdividePlanet(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices);
    void SpherisePlanet(std::vector<glm::vec3>& vertices);
    void InvertPlanet(std::vector<glm::vec3>& vertices);
};

class PlanetShader : public Shader {
public:
    PlanetShader(const char* vsSource, const char* fsSource, const char* shaderName);

private:
    void SetupUniforms();
    GLint planetColourLocation;
};