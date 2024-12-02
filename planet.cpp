#include "planet.h"

PlanetShader::PlanetShader(const char *vsSource, const char *fsSource, const char *shaderName) : Shader(vsSource, fsSource, shaderName) {};

Planet::Planet(PlanetShader* shader) : Object(shader) {
    std::vector<glm::vec3> vertices;
    vertices = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0, 0.0, 0.0),
        glm::vec3(1.0, 0.0, 1.0),
        glm::vec3(0.0, 0.0, 1.0)
    };

    indicesCount = 1;

    SetData(vertices, std::vector<glm::vec3>(0), std::vector<unsigned int>(0));
}

void Planet::Draw() {
  glBindVertexArray(VAO);
  glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}