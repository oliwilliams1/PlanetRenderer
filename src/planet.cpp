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

	GeneratePlanet(vertices);
	SetData(vertices);

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

void Planet::GeneratePlanet(std::vector<glm::vec3>& vertices) {
	int resolution = 6;

	std::vector<glm::vec3> tempPlane;

	for (int x = 0; x < resolution; x++) {
		for (int y = 0; y < resolution; y++) {
			// Specific order makes it possible for tess control shader to actually work
			tempPlane.emplace_back(-1.0f / 2.0f + 1.0f * x / (float)resolution, 0, -1.0f / 2.0f + 1.0f * (y + 1) / (float)resolution); // Bottom Left
			tempPlane.emplace_back(-1.0f / 2.0f + 1.0f * (x + 1) / (float)resolution, 0, -1.0f / 2.0f + 1.0f * (y + 1) / (float)resolution); // Bottom Right
			tempPlane.emplace_back(-1.0f / 2.0f + 1.0f * (x + 1) / (float)resolution, 0, -1.0f / 2.0f + 1.0f * y / (float)resolution); // Top Right
			tempPlane.emplace_back(-1.0f / 2.0f + 1.0f * x / (float)resolution, 0, -1.0f / 2.0f + 1.0f * y / (float)resolution); // Top Left
		}
	}

	// Bottom face
	std::vector<glm::vec3> bottomPlane = tempPlane;

	glm::mat4 translateDown = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f));

	// Apply translation to each vertex
	for (int i = 0; i < bottomPlane.size(); i++) {
		bottomPlane[i] = translateDown * glm::vec4(bottomPlane[i], 1.0f);
	}

	// Top face
	std::vector<glm::vec3> topPlane = tempPlane;

	glm::mat4 translateUp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));
	glm::mat4 rotate180AroundZ = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	// Apply rotation to each vertex
	for (int i = 0; i < topPlane.size(); i++) {
		topPlane[i] = translateUp * rotate180AroundZ * glm::vec4(topPlane[i], 1.0f);
	}

	// Right plane
	std::vector<glm::vec3> rightPlane = tempPlane;
	glm::mat4 translateNegZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -0.5f));
	glm::mat4 rotate90AroundX = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	for (int i = 0; i < rightPlane.size(); i++) {
		rightPlane[i] = translateNegZ * rotate90AroundX * glm::vec4(rightPlane[i], 1.0f);
	}

	// Left plane
	std::vector<glm::vec3> leftPlane = tempPlane;
	glm::mat4 translatePosZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f));
	glm::mat4 rotate270AroundX = glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	for (int i = 0; i < leftPlane.size(); i++) {
		leftPlane[i] = translatePosZ * rotate270AroundX * glm::vec4(leftPlane[i], 1.0f);
	}

	// Front plane
	std::vector<glm::vec3> frontPlane = tempPlane;
	glm::mat4 translatePosX = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.0f));
	glm::mat4 rotate90AroundY = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	for (int i = 0; i < frontPlane.size(); i++) {
		frontPlane[i] = translatePosX * rotate90AroundY * rotate270AroundX * glm::vec4(frontPlane[i], 1.0f);
	}

	// Back plane
	std::vector<glm::vec3> backPlane = tempPlane;
	glm::mat4 translateNegX = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, 0.0f));
	glm::mat4 rotate270AroundY = glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	for (int i = 0; i < backPlane.size(); i++) {
		backPlane[i] = translateNegX * rotate90AroundY * rotate90AroundX * glm::vec4(backPlane[i], 1.0f);
	}

	vertices.insert(vertices.end(), bottomPlane.begin(), bottomPlane.end());
	vertices.insert(vertices.end(), topPlane.begin(), topPlane.end());
	vertices.insert(vertices.end(), rightPlane.begin(), rightPlane.end());
	vertices.insert(vertices.end(), leftPlane.begin(), leftPlane.end());
	vertices.insert(vertices.end(), frontPlane.begin(), frontPlane.end());
	vertices.insert(vertices.end(), backPlane.begin(), backPlane.end());

	// For glDrawArrays
	indicesCount = 4 * resolution * resolution * 6;
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