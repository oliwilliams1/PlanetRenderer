#include "planet.h"

Planet::Planet() : Object() {
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
    GeneratePlanet(vertices, indices);

    SetData(vertices, indices);
}

void Planet::GeneratePlanet(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices) {
    vertices = {
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f),
        glm::vec3(1.0f,  1.0f, -1.0f),
        glm::vec3(-1.0f,  1.0f, -1.0f),
        glm::vec3(-1.0f, -1.0f,  1.0f),
        glm::vec3(1.0f, -1.0f,  1.0f),
        glm::vec3(1.0f,  1.0f,  1.0f),
        glm::vec3(-1.0f,  1.0f,  1.0f)
    };

    indices = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        0, 1, 5, 0, 5, 4,
        3, 2, 6, 3, 6, 7,
        0, 3, 7, 0, 7, 4,
        1, 2, 6, 1, 6, 5
    };
}