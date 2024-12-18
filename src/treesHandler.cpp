#include "treesHandler.h"

TreesHandler::TreesHandler(Planet* planet) {
    this->planet = planet;
    this->shader = new Shader("shaders/tree.vert", "shaders/tree.frag", "Trees Shader");

    int numPoints = 500;

    FibonacciSphere(numPoints);

    computeShaderProgram = CompileComputeShader("shaders/treeGen.comp");

    glGenBuffers(1, &inputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, inputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 3 * numPoints, vertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenTextures(1, &outputDataTexture);
    glBindTexture(GL_TEXTURE_2D, outputDataTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, numPoints, 1, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(computeShaderProgram);

    glBindImageTexture(0, outputDataTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, planet->noiseCubemapTexture);

    glUniform1i(glGetUniformLocation(computeShaderProgram, "u_numPoints"), numPoints);
    glUniform1i(glGetUniformLocation(computeShaderProgram, "u_NoiseCubemap"), 1);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputBuffer);

    glDispatchCompute((numPoints + 255) / 256, 1, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    SetupBuffers();
}

void TreesHandler::FibonacciSphere(int numPoints) {
	float goldenAngle = 3.14159265358 * (3.0 - sqrt(5.0));

	vertices.clear();

	for (int i = 0; i < numPoints; i++) {
		float theta = goldenAngle * i;
		float phi = acos(1.0 - 2.0 * ((float)i + 0.5) / numPoints);

		glm::vec3 dir(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));

		vertices.emplace_back(dir * planet->planetScale);
	}
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

TreesHandler::~TreesHandler() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}