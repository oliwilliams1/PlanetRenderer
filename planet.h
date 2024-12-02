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
};

class Planet : public Object {
public:
    Planet(PlanetShader* shader);
    void Draw() override;
};