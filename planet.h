#pragma once

#include <glm/glm.hpp>
#include "object.h"
#include "shader.h"

class Planet : public Object {
public:
	Planet(Shader* shader);
	void Draw() override;

private:
	void GeneratePlanet(std::vector<glm::vec3>& vertices);
};