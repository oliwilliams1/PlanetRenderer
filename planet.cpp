#include "planet.h"

Planet::Planet(Shader* shader) : Object(shader) {
	this->shader = shader;

	std::vector<glm::vec3> vertices;

	GeneratePlanet(vertices);
	SetData(vertices);
}

void Planet::GeneratePlanet(std::vector<glm::vec3>& vertices) {
	int resolution = 6;

	float step = 1.0f / resolution;

    for (int x = 0; x < resolution; x++) {
        for (int y = 0; y < resolution; y++) {
            float xPos = -1.0f + x * step;
            float yPos = -1.0f + y * step;

			vertices.emplace_back(xPos, 0, yPos);
			vertices.emplace_back(xPos + step, 0, yPos);
			vertices.emplace_back(xPos + step, 0, yPos + step);
			vertices.emplace_back(xPos, 0, yPos + step);
        }
    }

	indicesCount = 4 * resolution * resolution;
}

void Planet::Draw() {
	glBindVertexArray(VAO);
	glDrawArrays(GL_QUADS, 0, indicesCount);
	glBindVertexArray(0);
}