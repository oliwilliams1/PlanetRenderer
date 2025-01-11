#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <cstdint>
#include <glm/glm.hpp>
#include "assetManager.h"

void SerializeObjects(const std::vector<ObjectData>& objects, const std::string& filename);
std::vector<ObjectData> DeserializeObjects(const std::string& filename);