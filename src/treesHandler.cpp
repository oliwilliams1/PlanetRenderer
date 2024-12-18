#include "treesHandler.h"

TreesHandler::TreesHandler(Planet* planet) {
    this->planet = planet;
    this->shader = new Shader("shaders/tree.vert", "shaders/tree.frag", "Trees Shader");

    int numPoints = 5000;

    FibonacciSphereSampling(numPoints);
    for (int i = 0; i < std::min(5, static_cast<int>(vertices.size())); i++) {
        std::cout << "Vertex " << i << ": " << vertices[i].x << ", " << vertices[i].y << ", " << vertices[i].z << std::endl;
    }
    computeShaderProgram = CompileComputeShader("shaders/treeGen.comp");

    glGenBuffers(1, &inputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, inputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec3) * numPoints, vertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &outputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec3) * numPoints, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glUseProgram(computeShaderProgram);
    glUniform1i(glGetUniformLocation(computeShaderProgram, "u_numPoints"), numPoints);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, outputBuffer);
    glDispatchCompute((numPoints + 255) / 256, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    GLuint stagingBuffer;
    glGenBuffers(1, &stagingBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, stagingBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * numPoints, nullptr, GL_DYNAMIC_READ);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, stagingBuffer);
    glCopyBufferSubData(GL_SHADER_STORAGE_BUFFER, GL_ARRAY_BUFFER, 0, 0, sizeof(glm::vec3) * numPoints);

    glBindBuffer(GL_ARRAY_BUFFER, stagingBuffer);
    glm::vec3* data = (glm::vec3*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * numPoints, GL_MAP_READ_BIT);

	for (int i = 0; i < numPoints; i++) {
		vertices.push_back(data[i]);
	}

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &stagingBuffer);

    SetupBuffers();
}

void TreesHandler::FibonacciSphereSampling(int numPoints) {
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