#pragma once

#include <glm/glm.hpp>
#include <cmath>
#include <unordered_map>
#include <string>
#include "object.h"
#include "shader.h"

class PlanetShader : public Shader {
public:
    PlanetShader(const char* vsSource, const char* fsSource, const char* shaderName);

private:
    void SetupUniforms();
    GLint planetColourLocation, planetScaleLocation;
};

class Planet : public Object {
public:
    Planet(PlanetShader* shader);

    void Draw() override;
    void ObjectDebugImGUI() override;

private:
    float planetScale = 100.0f;

    GLint planetScaleLocation;
    PlanetShader* shader;

    void GeneratePlanet(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices);
    void SubdividePlanet(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices);
};