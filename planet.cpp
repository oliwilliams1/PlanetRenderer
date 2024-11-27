#include "planet.h"

Planet::Planet() : Object() {
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
    GeneratePlanet(vertices, indices);

    for (int i = 0; i < 3; i++) {
        SubdividePlanet(vertices, indices);
    }
    SpherisePlanet(vertices);
    InvertPlanet(vertices);

    std::vector<glm::vec3> normals(vertices.size());
    for (int i = 0; i < normals.size(); i++) {
        normals[i] = glm::normalize(vertices[i]);
    }

    SetData(vertices, normals, indices);
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
        4, 6, 5, 4, 7, 6,
        0, 4, 5, 0, 5, 1,
        3, 2, 6, 3, 6, 7,
        0, 3, 7, 0, 7, 4,
        1, 5, 6, 1, 6, 2 
    };
}

void Planet::SubdividePlanet(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices) {
    std::vector<glm::vec3> originalVertices = vertices;
    std::vector<unsigned int> originalIndices = indices;

    vertices.clear();
    indices.clear();

    for (size_t i = 0; i < originalIndices.size(); i += 3) {
        unsigned int v0 = originalIndices[i];
        unsigned int v1 = originalIndices[i + 1];
        unsigned int v2 = originalIndices[i + 2];

        glm::vec3 p0 = originalVertices[v0];
        glm::vec3 p1 = originalVertices[v1];
        glm::vec3 p2 = originalVertices[v2];

        for (int j = 0; j < 1; ++j) {
            glm::vec3 m0 = (p0 + p1) * 0.5f;
            glm::vec3 m1 = (p1 + p2) * 0.5f;
            glm::vec3 m2 = (p2 + p0) * 0.5f;

            vertices.push_back(p0);
            vertices.push_back(p1);
            vertices.push_back(p2);
            vertices.push_back(m0);
            vertices.push_back(m1);
            vertices.push_back(m2);

            unsigned int baseIndex = vertices.size() - 6;

            indices.push_back(baseIndex);
            indices.push_back(baseIndex + 3);
            indices.push_back(baseIndex + 5);

            indices.push_back(baseIndex + 3);
            indices.push_back(baseIndex + 1);
            indices.push_back(baseIndex + 4);

            indices.push_back(baseIndex + 5);
            indices.push_back(baseIndex + 4);
            indices.push_back(baseIndex + 2);

            indices.push_back(baseIndex + 3);
            indices.push_back(baseIndex + 4);
            indices.push_back(baseIndex + 5);

            p0 = vertices.back();
            p1 = vertices[vertices.size() - 2];
            p2 = vertices[vertices.size() - 1];
        }
    }
}

void Planet::SpherisePlanet(std::vector<glm::vec3>& vertices) {
    for (int i = 0; i < vertices.size(); i++) {
		vertices[i] = glm::normalize(vertices[i]);
	}
}

void Planet::InvertPlanet(std::vector<glm::vec3>& vertices) {
	for (int i = 0; i < vertices.size(); i++) {
		vertices[i] *= -1.0f;
	}
}