#include "treesHandler.h"

TreesHandler::TreesHandler(Planet* planet) {
	this->planet = planet;
    this->shader = new Shader("shaders/tree.vert", "shaders/tree.frag", "Trees Shader");
    this->noiseCubemapCPU = new Cubemap(planet->noiseCubemapTexture, planet->cubemapResolution);
	this->numSubdivisions = 50;

    PlaceTrees(numSubdivisions);
    SetupBuffers();
}

void TreesHandler::UpdateTrees() {
    delete noiseCubemapCPU;
    noiseCubemapCPU = new Cubemap(planet->noiseCubemapTexture, planet->cubemapResolution);
    PlaceTrees(numSubdivisions);

    glBindBuffer(GL_ARRAY_BUFFER, IBO);
    glBufferData(GL_ARRAY_BUFFER, m_ModelMatrices.size() * sizeof(glm::mat4), m_ModelMatrices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

    float treeScale = 2.0f;

    for (int i = 0; i < 18; i++) {
        vertices[i] *= treeScale;
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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

    // Load an icosphere to subdivide and place trees on
    LoadModel("resources/icosphere.obj", vertices, indices);

    // Iterate through every face, and attempt to place trees on it
    for (int i = 0; i < indices.size(); i += 3) {
        glm::vec3 v1 = glm::vec3(vertices[indices[i]]);
        glm::vec3 v2 = glm::vec3(vertices[indices[i + 1]]);
		glm::vec3 v3 = glm::vec3(vertices[indices[i + 2]]);

        PlaceTreesOnTriangle(numTrees, v1, v2, v3);
    }
}

void TreesHandler::AddTree(glm::vec3 dir, float height) {
    // Calculate the position of the tree
    glm::vec3 normal = glm::normalize(dir);
    glm::vec3 pos = normal * (planet->planetScale + 2.0f);
    pos += height * (planet->planetScale * planet->noiseAmplitude * normal);
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), pos);

    // Caluclate the realtive up coord, so the tree faces away from the planet
    glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
    glm::vec3 right = glm::normalize(glm::cross(up, normal));
    up = glm::cross(normal, right);

    // Constuct a rotation matrix
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation[0] = glm::vec4(right, 0.0f);
    rotation[1] = glm::vec4(up, 0.0f);
    rotation[2] = glm::vec4(-dir, 0.0f);
    rotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    // Construct a model matrix
    glm::mat4 m_Model = translation * rotation;
    m_ModelMatrices.push_back(m_Model);
}

void TreesHandler::PlaceTreesOnTriangle(int points, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
    std::mt19937 gen(planet->seed);
    std::uniform_real_distribution<> displacement(-0.002f, 0.002f); // Trees are scatterd, but not close enough to intersect
    
    std::uniform_real_distribution<> density(0.0f, 1.0f); // Random killing for tree density

    // Find vertical and horizontal offsets relative to the triangle
    glm::vec3 vOffset = (v2 - v1) / (float)points;
    glm::vec3 hOffset = (v3 - v2) / (float)points;

    // Iterate through vertical slices of the triangle
    for (int y = 0; y < points; y++) {
        // Iterate through horizontal slices of the triangle (based on vertical slices)
        for (int x = 0; x <= y; x++) {
            // Calculate the normalised direction of the tree from the center of the planet
            glm::vec3 dir = v1 + (float)y * vOffset + (float)x * hOffset;

            // Apply the random offsets
            dir += glm::vec3(displacement(gen), displacement(gen), displacement(gen));
            dir = glm::normalize(dir);

            // Sample the cubemap of the planet to see if tree can be there or not
            glm::vec3 colour = noiseCubemapCPU->Sample(dir);
            float height = colour.r;

            // If a tree can be there, consider adding based on density
            if (colour.g > 0.0f) {
                float densityValue = density(gen);

                if (densityValue < colour.g) {
					AddTree(dir, height);
				}

                // Increment the pass counter
                passCounter++;
            }
        }
    }
}

TreesHandler::~TreesHandler() {
    delete noiseCubemapCPU;
}