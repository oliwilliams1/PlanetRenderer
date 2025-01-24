#include "atmosphereRenderer.h"

Atmosphere::Atmosphere(float width, float height, GLuint gFragPos, GLuint gRendered) {
	this->width  = width;
	this->height = height;
	this->gRendered = gRendered;
	this->gFragPos = gFragPos;
	this->u_Steps = 5;
	this->u_MinAtmsDistance = 1000.0f;
	this->u_MaxAtmsDistance = 1250.0f;
	this->u_AtmsExpFalloff = 7.0f;
	this->u_FalloffB = 0.1f;
	this->u_ColourContribs = glm::vec3(0.25f, 0.7f, 1.0f);
	this->u_Colour1 = glm::vec3(1.0f, 0.0f, 0.0f);
	this->u_Colour2 = glm::vec3(0.0f, 1.0f, 0.0f);
	this->u_Colour3 = glm::vec3(0.0f, 0.0f, 1.0f);
	this->u_SunColour = glm::vec3(1.0f, 1.0f, 0.8f);
	this->u_SunIntensity = 5.0f;

	this->shader = new Shader("shaders/atmosphere.vert", "shaders/atmosphere.frag", "Atmosphere");
	SetupQuad();
	CreateTextures();
	
	shader->use();
	glUniform1i(GetUniformLocation(shader->shaderProgram, "u_FragColourIn"), 0);
	glUniform1i(GetUniformLocation(shader->shaderProgram, "u_FragPos"), 1);

	glUniform3f(GetUniformLocation(shader->shaderProgram, "u_PlanetCenter"),    0.0f, 0.0f, 0.0f);
	glUniform1i(GetUniformLocation(shader->shaderProgram, "u_STEPS"),           u_Steps);
	glUniform1f(GetUniformLocation(shader->shaderProgram, "u_MinAtmsDistance"), u_MinAtmsDistance);
	glUniform1f(GetUniformLocation(shader->shaderProgram, "u_MaxAtmsDistance"), u_MaxAtmsDistance);
	glUniform1f(GetUniformLocation(shader->shaderProgram, "u_AtmsExpFalloff"),  u_AtmsExpFalloff);
	glUniform1f(GetUniformLocation(shader->shaderProgram, "u_FalloffB"),        u_FalloffB);
	glUniform3f(GetUniformLocation(shader->shaderProgram, "u_ColourContribs"),  u_ColourContribs.x, u_ColourContribs.y, u_ColourContribs.z);
	glUniform3f(GetUniformLocation(shader->shaderProgram, "u_Colour1"),         u_Colour1.x, u_Colour1.y, u_Colour1.z);
	glUniform3f(GetUniformLocation(shader->shaderProgram, "u_Colour2"),         u_Colour2.x, u_Colour2.y, u_Colour2.z);
	glUniform3f(GetUniformLocation(shader->shaderProgram, "u_Colour3"),         u_Colour3.x, u_Colour3.y, u_Colour3.z);
	glUniform3f(GetUniformLocation(shader->shaderProgram, "u_SunColour"),       u_SunColour.x, u_SunColour.y, u_SunColour.z);
	glUniform1f(GetUniformLocation(shader->shaderProgram, "u_SunIntensity"),    u_SunIntensity);
}

void Atmosphere::SetupQuad() {
	float vertices[18] = {
		-1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);

	glBindVertexArray(quadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void Atmosphere::CreateTextures() {
	glGenFramebuffers(1, &gAtmosphereFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, gAtmosphereFBO);

	glBindTexture(GL_TEXTURE_2D, gRendered);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gRendered, 0);

	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Atmosphere::Draw() {
	glBindFramebuffer(GL_FRAMEBUFFER, gAtmosphereFBO);
	glViewport(0, 0, width, height);

	shader->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gRendered);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gFragPos);
	
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Atmosphere::DebugDraw() {
	ImGui::Text("Atmosphere");

	if (ImGui::SliderInt("res", &u_Steps, 1, 100)) { 
		shader->use(); glUniform1i(GetUniformLocation(shader->shaderProgram, "u_STEPS"), u_Steps); 
	}

	if (ImGui::SliderFloat("min", &u_MinAtmsDistance, 0.0, 10000.0)) {
		shader->use(); glUniform1f(GetUniformLocation(shader->shaderProgram, "u_MinAtmsDistance"), u_MinAtmsDistance);
	}
	
	if (ImGui::SliderFloat("max", &u_MaxAtmsDistance, 0.0, 10000.0)) {
		shader->use(); glUniform1f(GetUniformLocation(shader->shaderProgram, "u_MaxAtmsDistance"), u_MaxAtmsDistance);
	}

	if (ImGui::SliderFloat("falloff", &u_AtmsExpFalloff, 0.1, 20.0)) {
		shader->use(); glUniform1f(GetUniformLocation(shader->shaderProgram, "u_AtmsExpFalloff"), u_AtmsExpFalloff);
	}

	if (ImGui::SliderFloat("falloff b", &u_FalloffB, 0.0, 1.0)) {
		shader->use(); glUniform1f(GetUniformLocation(shader->shaderProgram, "u_FalloffB"), u_FalloffB);
	}

	if (ImGui::SliderFloat3("Colour contribs", &u_ColourContribs.x, 0.0, 1.0)) {
		shader->use(); glUniform3f(GetUniformLocation(shader->shaderProgram, "u_ColourContribs"), u_ColourContribs.x, u_ColourContribs.y, u_ColourContribs.z);
	}

	if (ImGui::ColorEdit3("Colour 1", &u_Colour1.x)) {
		shader->use(); glUniform3f(GetUniformLocation(shader->shaderProgram, "u_Colour1"), u_Colour1.x, u_Colour1.y, u_Colour1.z);
	}

	if (ImGui::ColorEdit3("Colour 2", &u_Colour2.x)) {
		shader->use(); glUniform3f(GetUniformLocation(shader->shaderProgram, "u_Colour2"), u_Colour2.x, u_Colour2.y, u_Colour2.z);
	}

	if (ImGui::ColorEdit3("Colour 3", &u_Colour3.x)) {
		shader->use(); glUniform3f(GetUniformLocation(shader->shaderProgram, "u_Colour3"), u_Colour3.x, u_Colour3.y, u_Colour3.z);
	}

	if (ImGui::ColorEdit3("Sun colour", &u_SunColour.x)) {
		shader->use(); glUniform3f(GetUniformLocation(shader->shaderProgram, "u_SunColour"), u_SunColour.x, u_SunColour.y, u_SunColour.z);
	}

	if (ImGui::SliderFloat("Sun intensity", &u_SunIntensity, 0.0, 100.0)) {
		shader->use(); glUniform1f(GetUniformLocation(shader->shaderProgram, "u_SunIntensity"), u_SunIntensity);
	}
}

Atmosphere::~Atmosphere() {
	glDeleteFramebuffers(1, &gAtmosphereFBO);
}