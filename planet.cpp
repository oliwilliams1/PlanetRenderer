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
	glDrawArrays(GL_PATCHES, 0, indicesCount);
	glBindVertexArray(0);
}

PlanetShader::PlanetShader(const char* vsSource, const char* fsSource, const char* shaderName) : Shader(vsSource, fsSource, shaderName) {
	AddTesselationShaders();
}

void PlanetShader::AddTesselationShaders() {
    std::string tesc, tese;
    if (!ReadFile("shaders/planet.tesc", tesc)) exit(1);
    AddShader(shaderProgram, tesc.c_str(), GL_TESS_CONTROL_SHADER);

    if (!ReadFile("shaders/planet.tese", tese)) exit(1);
    AddShader(shaderProgram, tese.c_str(), GL_TESS_EVALUATION_SHADER);

    GLint success = 0;
    GLchar errorLog[1024] = { 0 };

    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(shaderProgram, sizeof(errorLog), nullptr, errorLog);
        std::cout << "Error linking shader program: " << errorLog << std::endl;
        exit(1);
    }

    glUseProgram(shaderProgram);
}