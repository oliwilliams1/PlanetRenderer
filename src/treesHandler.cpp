#include "treesHandler.h"

TreesHandler::TreesHandler(Planet* planet) {
	this->planet = planet;
	this->shader = new Shader("shaders/tree.vert", "shaders/tree.frag", "Trees Shader");
	this->noiseCubemapCPU = new Cubemap(planet->noiseCubemapTexture, planet->cubemapResolution);
	this->numSubdivisions = 50;
	this->treeScale = 7.5f;
	albedoLocation = GetUniformLocation(shader->shaderProgram, "u_Albedo");
	normalLocation = GetUniformLocation(shader->shaderProgram, "u_Normal");

	treeScaleLocation = GetUniformLocation(shader->shaderProgram, "u_TreeScale");
	glUniform1f(treeScaleLocation, treeScale);

	cullingComputeShader = CompileComputeShader("shaders/tree.comp");

	PlaceTrees(numSubdivisions);
	SetupBuffers();
	CullTrees();
	CreateTextures();
}

void TreesHandler::UpdateTrees() {
	delete noiseCubemapCPU;
	noiseCubemapCPU = new Cubemap(planet->noiseCubemapTexture, planet->cubemapResolution);
	PlaceTrees(numSubdivisions);
}

void TreesHandler::DebugDraw() {
	if (ImGui::SliderFloat("Tree Scale", &treeScale, 0.1f, 10.0f)) {
		shader->use();
		glUniform1f(treeScaleLocation, treeScale);
	};
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

	glGenBuffers(1, &TreeInputBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, TreeInputBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, instanceData.size() * sizeof(glm::vec4), instanceData.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &TreeOutputBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, TreeOutputBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, instanceData.size() * sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &AtomicCounterBuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, AtomicCounterBuffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), (unsigned int*)0, GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
}

void TreesHandler::CullTrees() {
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), (unsigned int*)0, GL_DYNAMIC_DRAW);

	GLuint* counterPtr = (GLuint*)glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_WRITE_ONLY);
	*counterPtr = 0;

	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, TreeOutputBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, TreeInputBuffer);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 2, AtomicCounterBuffer);

	glUseProgram(cullingComputeShader);
	glDispatchCompute((instanceData.size() + 31) / 32, 1, 1);
}

void TreesHandler::CreateTextures() {
	LoadTexture(&texturesTree0.topAlbedo, "resources/trees/tree0-albedo-imposter.png");
	LoadTexture(&texturesTree0.topNormal, "resources/trees/tree0-normal-imposter.png");
}

void TreesHandler::Draw() {
	shader->use();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, TreeOutputBuffer);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texturesTree0.topAlbedo);
	glUniform1i(albedoLocation, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texturesTree0.topNormal);
	glUniform1i(normalLocation, 1);

	GLuint* counterData = (GLuint*)glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_READ_ONLY);
	visibleTrees = counterData[0];
	glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);

	glBindVertexArray(VAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, visibleTrees);
	
	glBindVertexArray(0);
}

void TreesHandler::PlaceTrees(int numTrees) {
	instanceData.clear();

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
	glm::vec3 pos = normal * (planet->planetScale + treeScale);
	pos += height * (planet->planetScale * planet->noiseAmplitude * normal);

	instanceData.emplace_back(pos, 0.0f);
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