#include "planet.h"
#include "assetManager.h"

Planet::Planet(App* app, Shader* shader) : Object(shader) {
	this->app = app;
	this->shader = shader;
	this->noiseAmplitude = 0.1f;
	this->terrainLevels = glm::vec3(0.01f, 0.08f, 0.3f);
	this->planetScale = 1000.0f;
	this->cubemapResolution = 512;
	this->seed = 0;
	this->needToDispatch = false;
	this->lastDispatchTime = 0.0f;
	this->camera = app->camera;

	this->noiseGen = new Noise(cubemapResolution, &noiseCubemapTexture, &normalCubemapTexture);	
	noiseGen->Dispatch(seed);

	this->treesHandler = new TreesHandler(this);

	SetMesh("planet");

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

void Planet::Draw() {
	UpdateModelMatrix();
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
	if (ImGui::SliderFloat3("Position", &position.x, -10.0f, 10.0f)) UpdateModelMatrix();
	if (ImGui::SliderFloat3("Rotation", &rotation.x, -180.0f, 180.0f)) UpdateModelMatrix();
	if (ImGui::SliderFloat("Planet Scale", &planetScale, -1.0f, 1000.0f)) glUniform1f(planetScaleLocation, planetScale);
	if (ImGui::SliderFloat("Noise Amplitude", &noiseAmplitude, 0.0f, 0.5f)) glUniform1f(noiseAmplitudeLocation, noiseAmplitude);
	if (ImGui::SliderFloat("Max water level", &terrainLevels.x, -1.0f, 1.0f, "%.2f")) glUniform3f(terrainLevelsLocation, terrainLevels.x, terrainLevels.y, terrainLevels.z);
	if (ImGui::SliderFloat("Max sand level",  &terrainLevels.y, -1.0f, 1.0f, "%.2f")) glUniform3f(terrainLevelsLocation, terrainLevels.x, terrainLevels.y, terrainLevels.z);
	if (ImGui::SliderFloat("Max grass level", &terrainLevels.z, -1.0f, 1.0f, "%.2f")) glUniform3f(terrainLevelsLocation, terrainLevels.x, terrainLevels.y, terrainLevels.z);
	if (ImGui::SliderInt("Seed", &seed, 0, 1000)) needToDispatch = true;

	if (needToDispatch) {
		float currentTime = glfwGetTime();
		if (currentTime - lastDispatchTime > 0.2f) {
			noiseGen->Dispatch(seed);
			needToDispatch = false;
			lastDispatchTime = glfwGetTime();
			treesHandler->UpdateTrees();
		}
	}

	treesHandler->DebugDraw();
}