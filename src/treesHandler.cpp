#include "treesHandler.h"
#include "assetManager.h"

TreesHandler::TreesHandler(Planet* planet) {
	this->planet = planet;
	this->imposterShader = new Shader("shaders/treeImp.vert", "shaders/treeImp.frag", "Imposter tree shader");
	this->treeShader = new Shader("shaders/tree.vert", "shaders/tree.frag", "Tree shader");
	this->noiseCubemapCPU = new Cubemap(planet->noiseCubemapTexture, planet->cubemapResolution);
	this->numSubdivisions = 50;
	this->treeScale = 8.0f;

	imposterShader->use();
	albedoLocation = GetUniformLocation(imposterShader->shaderProgram, "u_Albedo");
	normalLocation = GetUniformLocation(imposterShader->shaderProgram, "u_Normal");

	treeScaleLocation = GetUniformLocation(imposterShader->shaderProgram, "u_TreeScale");
	glUniform1f(treeScaleLocation, treeScale);

	treeShader->use();
	TreeSSSLocation = GetUniformLocation(treeShader->shaderProgram, "u_SSS");

	PlaceTrees(numSubdivisions);
	SetupBuffers();
	CreateTextures();
}

void TreesHandler::UpdateTrees() {
	delete noiseCubemapCPU;
	noiseCubemapCPU = new Cubemap(planet->noiseCubemapTexture, planet->cubemapResolution);
	PlaceTrees(numSubdivisions);

	glBindBuffer(GL_ARRAY_BUFFER, ImposterPBO);
	glBufferData(GL_ARRAY_BUFFER, instancePositions.size() * sizeof(glm::vec3), instancePositions.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TreesHandler::DebugDraw() {
	if (ImGui::SliderFloat("Tree Scale", &treeScale, 0.1f, 10.0f)) {
		imposterShader->use();
		glUniform1f(treeScaleLocation, treeScale);
	};
}

void TreesHandler::SetupBuffers() {
	float vertices[18] = {
		-1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};

	glGenVertexArrays(1, &ImposterVAO);
	glBindVertexArray(ImposterVAO);

	glGenBuffers(1, &ImposterVBO);
	glBindBuffer(GL_ARRAY_BUFFER, ImposterVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Imposter matricies in SSBO
	glGenBuffers(1, &ImposterMBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ImposterMBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, instanceData.size() * sizeof(glm::mat4), instanceData.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glBindVertexArray(0);

	glGenBuffers(1, &ImposterMatrixIndexBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ImposterMatrixIndexBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, instanceIndexes.size() * sizeof(unsigned int), instanceIndexes.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	tree.push_back(Object(treeShader));
	tree.push_back(Object(treeShader));
	tree[0].SetMesh("tree0");
	tree[1].SetMesh("tree0_1", true);

	treeShader->use();
	for (int i = 0; i < tree.size(); i++) {
		tree[i].scale = glm::vec3(0.02f);
		tree[i].UpdateModelMatrix();
	}
}

void TreesHandler::CreateTextures() {
	LoadTexture(&texturesTree0.topAlbedo, "resources/tree0-albedo-imp.png");
	LoadTexture(&texturesTree0.topNormal, "resources/tree0-normal-imp.png");
}

void TreesHandler::Draw() {
	imposterShader->use();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ImposterMBO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texturesTree0.topAlbedo);
	glUniform1i(albedoLocation, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texturesTree0.topNormal);
	glUniform1i(normalLocation, 1);

	glBindVertexArray(ImposterVAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, instancePositions.size());
	glBindVertexArray(0);

	glDisable(GL_CULL_FACE);
	treeShader->use();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ImposterMBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ImposterMatrixIndexBuffer);

	glm::vec3 camPos = planet->camera->position;
	float dist = 85.0f;

	instanceIndexes.clear();
	for (int i = 0; i < instancePositions.size(); i++) {
		if (glm::distance(instancePositions[i], camPos) < dist) instanceIndexes.emplace_back(i);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ImposterMatrixIndexBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, instanceIndexes.size() * sizeof(unsigned int), instanceIndexes.data(), GL_DYNAMIC_DRAW);

	glActiveTexture(GL_TEXTURE0);

	for (int i = 0; i < tree.size(); i++) {
		if (i == 0) {
			// Bark, dont want SSS
			glUniform1i(TreeSSSLocation, 0);
		}
		else {
			// Leaves, want SSS
			glUniform1i(TreeSSSLocation, 1);
		}
		
		glBindTexture(GL_TEXTURE_2D, tree[i].albedo);
		glUniform1i(tree[i].albedoLocation, 0);

		glBindVertexArray(tree[i].VAO);
		glDrawElementsInstanced(GL_TRIANGLES, tree[i].indicesCount, GL_UNSIGNED_INT, 0, instanceIndexes.size());
		glBindVertexArray(0);
	}
	glEnable(GL_CULL_FACE);
}

void TreesHandler::PlaceTrees(int numTrees) {
	instancePositions.clear();

	ObjectData object;

	AssetManager::System::GetInstance().GetObject("icosphere", object);

	// Iterate through every face, and attempt to place trees on it
	for (int i = 0; i < object.indices.size(); i += 3) {
		glm::vec3 v1 = glm::vec3(object.vertices[object.indices[i]]);
		glm::vec3 v2 = glm::vec3(object.vertices[object.indices[i + 1]]);
		glm::vec3 v3 = glm::vec3(object.vertices[object.indices[i + 2]]);

		PlaceTreesOnTriangle(numTrees, v1, v2, v3);
	}
}

void TreesHandler::AddTree(glm::vec3 dir, float height) {
	// Calculate the position of the tree
	glm::vec3 normal = glm::normalize(dir);
	glm::vec3 pos = normal * planet->planetScale;
	pos += height * (planet->planetScale * planet->noiseAmplitude * normal);
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), pos);

	instancePositions.emplace_back(pos);

	// Align the model matrix to the normal of the planet
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 right = glm::normalize(glm::cross(up, normal));
	up = glm::cross(normal, right);

	glm::mat4 rotation = glm::mat4(1.0f);
	rotation[0] = glm::vec4(right, 0.0f);
	rotation[1] = glm::vec4(normal, 0.0f);
	rotation[2] = glm::vec4(up, 0.0f);
	rotation[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 m_Model = translation * rotation;
	instanceData.emplace_back(m_Model);
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