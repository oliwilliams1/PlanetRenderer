#include "planet.h"

Planet::Planet(Shader* shader) : Object(shader) {
	this->shader = shader;

	std::vector<glm::vec3> vertices;

	GeneratePlanet(vertices);
	SetData(vertices);
}

void Planet::GeneratePlanet(std::vector<glm::vec3>& vertices) {
    int resolution = 6;

    float width = 50.0f;
    float height = 50.0f;

    for (int x = 0; x < resolution; x++) {
        for (int y = 0; y < resolution; y++) {
            // Specific order makes it possible for tess control shader to actually work
            vertices.emplace_back(-width / 2.0f + width * x / (float)resolution, 0, -height / 2.0f + height * (y + 1) / (float)resolution); // Bottom Left
            vertices.emplace_back(-width / 2.0f + width * (x + 1) / (float)resolution, 0, -height / 2.0f + height * (y + 1) / (float)resolution); // Bottom Right
            vertices.emplace_back(-width / 2.0f + width * (x + 1) / (float)resolution, 0, -height / 2.0f + height * y / (float)resolution); // Top Right
            vertices.emplace_back(-width / 2.0f + width * x / (float)resolution, 0, -height / 2.0f + height * y / (float)resolution); // Top Left
        }
    }

    // Worst bug to debug: took hours to find
    // Flip it around, it cannot be around x axis for some unknown reason not even gods know why
    glm::mat4 rotateAroundZ = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Apply rotation to each vertex
    for (int i = 0; i < vertices.size(); i++) {
        vertices[i] = rotateAroundZ * glm::vec4(vertices[i], 1.0f);
    }

    // For glDrawArrays
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