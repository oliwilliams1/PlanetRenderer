#pragma once

#include "object.h"

class Planet : public Object {
public:
    Planet();
private: 
    void GeneratePlanet(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices);
};