#include "treesHandler.h"

TreesHandler::TreesHandler(Planet* planet) {
	this->planet = planet;
    this->shader = new Shader("shaders/tree.vert", "shaders/tree.frag", "Trees Shader");
    this->noiseCubemapCPU = new Cubemap(planet->noiseCubemapTexture, planet->cubemapResolution);
	int numPoints = 50000;

	FibonacciSphere(numPoints);
    SetupBuffers();
}

void TreesHandler::SetupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void TreesHandler::Draw() {
    shader->use();
    glBindVertexArray(VAO);
    glPointSize(10.0f);
    glDrawArrays(GL_POINTS, 0, vertices.size());
    glBindVertexArray(0);
}

void TreesHandler::FibonacciSphere(int numPoints) {
	float goldenAngle = 3.14159265358 * (3.0 - sqrt(5.0));

	vertices.clear();

	for (int i = 0; i < numPoints; i++) {
		float theta = goldenAngle * i;
		float phi = acos(1.0 - 2.0 * ((float)i + 0.5) / numPoints);

		glm::vec3 dir(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));

        glm::vec3 colour = noiseCubemapCPU->Sample(dir);
        float height = colour.r;

        if (colour.g > 0.5f) {
            glm::vec3 normal = glm::normalize(dir);
            glm::vec3 pos = normal * planet->planetScale;
            pos += height * (planet->planetScale * planet->planetScale * normal);
			vertices.emplace_back(pos);
        }
	}
}

TreesHandler::~TreesHandler() {
    delete noiseCubemapCPU;
}