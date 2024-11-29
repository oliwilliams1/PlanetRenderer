#pragma once

#include <glm/glm.hpp>
#include <cmath>
#include <unordered_map>
#include <string>
#include <cstdint>
#include "object.h"
#include "shader.h"
#include "PerlinNoise.hpp"

class PlanetShader : public Shader {
public:
    PlanetShader(const char* vsSource, const char* fsSource, const char* shaderName);

private:
    void SetupUniforms();
    GLuint planetColourLocation, planetScaleLocation;
};

class Planet : public Object {
public:
    Planet(PlanetShader* shader);

    void Draw() override;
    void ObjectDebugImGUI() override;

private:
    float planetScale = 100.0f;

    GLuint planetScaleLocation, planetTextureLocation, planetTextureID;
    PlanetShader* shader;

    void GeneratePlanet(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices);
    void SubdividePlanet(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices);
    void GenerateTexture();
};