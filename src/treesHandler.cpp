#include "treesHandler.h"

TreesHandler::TreesHandler(Planet* planet) {
	this->planet = planet;
    this->shader = new Shader("shaders/tree.vert", "shaders/tree.frag", "Trees Shader");
    this->noiseCubemapCPU = new Cubemap(planet->noiseCubemapTexture, planet->cubemapResolution);
	int numTrees = 100000;

    PlaceTrees(numTrees);
    SetupBuffers();
}

void TreesHandler::SetupBuffers() {
    float vertices[18] = {
        -1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint IBO;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ARRAY_BUFFER, IBO);
    glBufferData(GL_ARRAY_BUFFER, m_ModelMatrices.size() * sizeof(glm::mat4), m_ModelMatrices.data(), GL_STATIC_DRAW);
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(i + 1);
        glVertexAttribPointer(i + 1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glVertexAttribDivisor(i + 1, 1);
    }
    glBindVertexArray(0);
}

void TreesHandler::Draw() {
    shader->use();
    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, m_ModelMatrices.size());
    glBindVertexArray(0);
}

void TreesHandler::PlaceTrees(int numTrees) {
    m_ModelMatrices.clear();

    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    LoadModel("resources/icosphere.obj", vertices, indices);

    for (int i = 0; i < indices.size(); i += 3) {
        glm::vec3 v1 = glm::vec3(vertices[indices[i]]);
        glm::vec3 v2 = glm::vec3(vertices[indices[i + 1]]);
		glm::vec3 v3 = glm::vec3(vertices[indices[i + 2]]);

        v1 *= 1000.0f;
        v2 *= 1000.0f;
        v3 *= 1000.0f;

        PlaceTreesOnTriangle(10, v1, v2, v3);
    }
}

void TreesHandler::PlaceTreesOnTriangle(int points, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
    glm::vec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 right = glm::normalize(glm::cross(up, normal));
    up = glm::normalize(glm::cross(normal, right));

    glm::mat4 rotation = glm::mat4(1.0f);
    rotation[0] = glm::vec4(right, 0.0f);
    rotation[1] = glm::vec4(up, 0.0f);
    rotation[2] = glm::vec4(-normal, 0.0f);
    rotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    glm::vec3 vOffset = (v2 - v1) / (float)points;
    glm::vec3 hOffset = (v3 - v2) / (float)points;

    for (int y = 0; y < points; y++) {
        for (int x = 0; x <= y; x++) {
            glm::vec3 position = v1 + (float)y * vOffset + (float)x * hOffset;
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);

            m_ModelMatrices.push_back(translation * rotation);
        }
    }
}

TreesHandler::~TreesHandler() {
    delete noiseCubemapCPU;
}