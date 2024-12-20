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

    float goldenAngle = 3.14159265358 * (3.0 - sqrt(5.0));

    for (int i = 0; i < numTrees; i++) {
        float theta = goldenAngle * i;
        float phi = acos(1.0 - 2.0 * ((float)i + 0.5) / numTrees);

        glm::vec3 dir(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));

        glm::vec3 colour = noiseCubemapCPU->Sample(dir);
        float height = colour.r;

        if (colour.g > 0.5f) {
            glm::vec3 normal = glm::normalize(dir);
            glm::vec3 pos = normal * planet->planetScale;
            pos += height * (planet->planetScale * planet->noiseAmplitude * normal);
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), pos);

            glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
            glm::vec3 right = glm::normalize(glm::cross(up, normal));
            up = glm::cross(normal, right);

            glm::mat4 rotation = glm::mat4(1.0f);
            rotation[0] = glm::vec4(right, 0.0f);
            rotation[1] = glm::vec4(up, 0.0f);
            rotation[2] = glm::vec4(-dir, 0.0f);
            rotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

            glm::mat4 m_Model = translation * rotation;
            m_ModelMatrices.push_back(m_Model);
        }
	}
}

TreesHandler::~TreesHandler() {
    delete noiseCubemapCPU;
}