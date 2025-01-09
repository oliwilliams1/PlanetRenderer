#include "deferredRenderer.h"

DeferredRenderer::DeferredRenderer(int width, int height) {
	this->viewportTexture = 0;
	this->wireframe = false;
	this->width = width;
	this->height = height;

	InitG_Buffer();
	InitDeferredShadingBuffer();

	deferredShader = new Shader("shaders/deferred.vert", "shaders/deferred.frag", "Deferred Shader");
	SetupQuad();

	deferredShader->use();
	glUniform1i(glGetUniformLocation(deferredShader->shaderProgram, "gPosition"), 1);
	glUniform1i(glGetUniformLocation(deferredShader->shaderProgram, "gNormal"),   2);
	glUniform1i(glGetUniformLocation(deferredShader->shaderProgram, "gAlbedo"),   3);
	glUniform1i(glGetUniformLocation(deferredShader->shaderProgram, "gObjectID"), 4);
}

void DeferredRenderer::SetupQuad() {
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

void DeferredRenderer::InitG_Buffer() {
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

	glGenTextures(1, &gObjectType);
	glBindTexture(GL_TEXTURE_2D, gObjectType);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8I, width, height, 0, GL_RED_INTEGER, GL_INT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gObjectType, 0);

	glGenTextures(1, &gDepth);
	glBindTexture(GL_TEXTURE_2D, gDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gDepth, 0);

	GLenum drawBuffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, drawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::InitDeferredShadingBuffer() {
	glGenVertexArrays(1, &quadVAO);
	glGenFramebuffers(1, &fboShading);
	glBindFramebuffer(GL_FRAMEBUFFER, fboShading);

	glGenTextures(1, &mainTexture);
	glBindTexture(GL_TEXTURE_2D, mainTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mainTexture, 0);

	GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::Bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glViewport(0, 0, width, height);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gObjectType);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void DeferredRenderer::Render() {
	glBindFramebuffer(GL_FRAMEBUFFER, fboShading);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	deferredShader->use();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mainTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, gObjectType);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::DebugDraw(bool flipUv) {
	float aspectRatio = (float)width / (float)height;
	int width = 175;
	int height = (int)(width / aspectRatio);

	ImGui::Combo("G-Buffer", &viewportTexture, "Rendered\0Position\0Normal\0Albedo\0\0");

	ImVec2 uv0 = flipUv ? ImVec2(0.0f, 0.0f) : ImVec2(0.0f, 1.0f);
	ImVec2 uv1 = flipUv ? ImVec2(1.0f, 1.0f) : ImVec2(1.0f, 0.0f);

	ImGui::Image((ImTextureID)(intptr_t)gPosition, ImVec2(width, height), uv0, uv1);
	ImGui::SameLine();
	ImGui::Image((ImTextureID)(intptr_t)gNormal, ImVec2(width, height), uv0, uv1);

	ImGui::Image((ImTextureID)(intptr_t)gAlbedo, ImVec2(width, height), uv0, uv1);
	ImGui::SameLine();
	ImGui::Image((ImTextureID)(intptr_t)mainTexture, ImVec2(width, height), uv0, uv1);
}

void DeferredRenderer::DisplayViewportImGui(glm::vec2 inputSize, bool flipUv) {
	ImGui::Checkbox("Wireframe", &wireframe);

	if (wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	ImVec2 size = ImVec2(width, height);

	if (inputSize.x != 0.0f && inputSize.y != 0.0f) {
		size = ImVec2(inputSize.x, inputSize.y);
	}

	ImVec2 uv0 = flipUv ? ImVec2(0.0f, 0.0f) : ImVec2(0.0f, 1.0f);
	ImVec2 uv1 = flipUv ? ImVec2(1.0f, 1.0f) : ImVec2(1.0f, 0.0f);

	GLuint texture = viewportTexture == 0 ? mainTexture : viewportTexture == 1 ? gPosition : viewportTexture == 2 ? gNormal : gAlbedo;
	ImGui::Image((ImTextureID)(intptr_t)texture, size, uv0, uv1);
}

DeferredRenderer::~DeferredRenderer() {
	glDeleteFramebuffers(1, &gBuffer);
	glDeleteFramebuffers(1, &fboShading);

	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormal);
	glDeleteTextures(1, &gAlbedo);
	glDeleteTextures(1, &mainTexture);
}