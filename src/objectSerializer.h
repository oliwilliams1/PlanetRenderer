#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <cstdint>
#include <glm/glm.hpp>

struct ObjectData {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
};

void SerializeObjects(const std::vector<ObjectData>& objects, const std::string& filename);
std::vector<ObjectData> DeserializeObjects(const std::string& filename);