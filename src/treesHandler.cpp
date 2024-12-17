#include "treesHandler.h"

TreesHandler::TreesHandler(Planet* planet) {
	this->planet = planet;
    this->shader = new Shader("shaders/tree.vert", "shaders/tree.frag", "Trees Shader");

	cubemapResolution = planet->cubemapResolution;

    SetupBuffers();
	RetrieveCubemapData();

    FibonacciSphereSampling(1000);
}

void TreesHandler::RetrieveCubemapData() {
	cubemapData = std::vector<float>(cubemapResolution * cubemapResolution * 6 * 4);

	glBindTexture(GL_TEXTURE_CUBE_MAP, planet->noiseCubemapTexture);

	for (int i = 0; i < 6; i++) {
		glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, GL_FLOAT, &cubemapData[static_cast<std::vector<float, std::allocator<float>>::size_type>((GLuint)i) * cubemapResolution * cubemapResolution * 4]);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

glm::vec4 TreesHandler::SampleCubemap(const glm::vec3& direction) {
    glm::vec3 dir = glm::normalize(direction);

    int face;
    float s, t;

    if (abs(dir.x) >= abs(dir.y) && abs(dir.x) >= abs(dir.z)) {
        face = (dir.x > 0) ? 0 : 1;
        s = -dir.z;
        t = dir.y;
    }
    else if (abs(dir.y) >= abs(dir.x) && abs(dir.y) >= abs(dir.z)) {
        face = (dir.y > 0) ? 2 : 3;
        s = dir.x;
        t = -dir.z;
    }
    else {
        face = (dir.z > 0) ? 4 : 5;
        s = dir.x;
        t = dir.y;
    }

    s = (s + 1.0f) * 0.5f * (cubemapResolution - 1);
    t = (t + 1.0f) * 0.5f * (cubemapResolution - 1);

    int pixelIndex = (face * cubemapResolution * cubemapResolution + int(t) * cubemapResolution + int(s)) * 4;

    return glm::vec4(
        cubemapData[pixelIndex],
        cubemapData[pixelIndex + 1],
        cubemapData[pixelIndex + 2],
        cubemapData[pixelIndex + 3]
    );
}

void TreesHandler::FibonacciSphereSampling(int numPoints) {
    float goldenAngle = 3.14159265358 * (3.0 - sqrt(5.0));

    for (int i = 0; i < numPoints; i++) {
        float theta = goldenAngle * i;
        float phi = acos(1.0 - 2.0 * ((float)i + 0.5) / numPoints);

        glm::vec3 dir(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));

        float result = SampleCubemap(dir).g;

        if (result > 0.5f) {
            glm::vec3 pos = glm::vec3(result * 1000.0f);

            glm::mat4 translation = glm::translate(glm::mat4(1.0f), pos);

            glm::mat4 m_Matrix = translation;

            trees.emplace_back(m_Matrix, pos);
        }
    }
};

void TreesHandler::SetupBuffers() {
    float vertices[18] = {
        -1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f
    };

    glGenVertexArrays(1, &treeVAO);
    glGenBuffers(1, &treeVBO);

    glBindVertexArray(treeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, treeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void TreesHandler::Draw() {
    shader->use();
	glBindVertexArray(treeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

TreesHandler::~TreesHandler() {
	glDeleteVertexArrays(1, &treeVAO);
	glDeleteBuffers(1, &treeVBO);
}