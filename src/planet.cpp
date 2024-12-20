#include "planet.h"

Planet::Planet(App* app, Shader* shader) : Object(shader) {
	this->app = app;
	this->shader = shader;
	this->noiseAmplitude = 0.1f;
	this->terrainLevels = glm::vec3(0.01f, 0.08f, 0.3f);
	this->planetScale = 1000.0f;

	app->noise->Dispatch();
	this->cubemapResolution = app->noise->cubemapResolution;
	this->noiseCubemapTexture  = app->noise->cubemapNoiseTexture;
	this->normalCubemapTexture = app->noise->cubemapNormalTexture;

	this->treesHandler = new TreesHandler(this);

	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;

	LoadPlanet(vertices, indices);
	SetData(vertices, std::vector<glm::vec3>(0), indices);

	shader->use();

	noiseCubemapLocation = GetUniformLocation(shader->shaderProgram, "u_NoiseCubemap");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, noiseCubemapTexture);

	normalCubemapLocation = GetUniformLocation(shader->shaderProgram, "u_NormalCubemap");
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, normalCubemapTexture);

	planetScaleLocation = GetUniformLocation(shader->shaderProgram , "u_PlanetScale");
	noiseAmplitudeLocation = GetUniformLocation(shader->shaderProgram, "u_Amplitude");
	terrainLevelsLocation = GetUniformLocation(shader->shaderProgram, "u_TerrainLevels");

	glUniform1i(noiseCubemapLocation, 0);
	glUniform1i(normalCubemapLocation, 1);
	glUniform1f(planetScaleLocation, planetScale);
	glUniform3f(terrainLevelsLocation, terrainLevels.x, terrainLevels.y, terrainLevels.z);
	glUniform1f(noiseAmplitudeLocation, noiseAmplitude);
}

void Planet::LoadPlanet(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices) {
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile("resources/planet.obj", 0);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[i];

		for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
			vertices.emplace_back(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);
		}

		for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
			aiFace face = mesh->mFaces[j];
			if (face.mNumIndices == 4) {
				indices.emplace_back(face.mIndices[0]);
				indices.emplace_back(face.mIndices[1]);
				indices.emplace_back(face.mIndices[2]);
				indices.emplace_back(face.mIndices[3]);
			}
		}
	}

	indicesCount = indices.size();
}

void Planet::Draw() {
	glBindVertexArray(VAO);
	glDrawArrays(GL_PATCHES, 0, indicesCount);
	glBindVertexArray(0);

	treesHandler->Draw();
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

void Planet::DebugDraw() {
	shader->use();
	ImGui::Begin("Planet data");
	if (ImGui::SliderFloat3("Position", &position.x, -10.0f, 10.0f)) UpdateModelMatrix();
	if (ImGui::SliderFloat3("Rotation", &rotation.x, -180.0f, 180.0f)) UpdateModelMatrix();
	if (ImGui::SliderFloat("Planet Scale", &planetScale, -1.0f, 1000.0f)) glUniform1f(planetScaleLocation, planetScale);
	if (ImGui::SliderFloat("Noise Amplitude", &noiseAmplitude, 0.0f, 0.5f)) glUniform1f(noiseAmplitudeLocation, noiseAmplitude);
	if (ImGui::SliderFloat("Max water level", &terrainLevels.x, -1.0f, 1.0f, "%.2f")) glUniform3f(terrainLevelsLocation, terrainLevels.x, terrainLevels.y, terrainLevels.z);
	if (ImGui::SliderFloat("Max sand level",  &terrainLevels.y, -1.0f, 1.0f, "%.2f")) glUniform3f(terrainLevelsLocation, terrainLevels.x, terrainLevels.y, terrainLevels.z);
	if (ImGui::SliderFloat("Max grass level", &terrainLevels.z, -1.0f, 1.0f, "%.2f")) glUniform3f(terrainLevelsLocation, terrainLevels.x, terrainLevels.y, terrainLevels.z);
	ImGui::End();
}