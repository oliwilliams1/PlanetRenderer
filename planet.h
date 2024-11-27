#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include "object.h"

class Planet : public Object {
public:
    Planet();
private: 
    void GeneratePlanet(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices);
    void SubdividePlanet(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices);
    void SpherisePlanet(std::vector<glm::vec3>& vertices);
    void InvertPlanet(std::vector<glm::vec3>& vertices);
};