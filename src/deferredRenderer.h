#pragma once

#include "app.h"

class App;
class Shader;

class DeferredRenderer {
public:
	DeferredRenderer(int width, int height);
	~DeferredRenderer();

	void Bind();
	void DebugDraw(bool flipUv = false);
	void DisplayViewportImGui(glm::vec2 size = { 0.0f, 0.0f }, bool flipUv = false);
	void Render();

	GLuint gBuffer, gPosition, gNormal, gAlbedo, gObjectType, gDepth;
	GLuint fboShading, mainTexture;
	int width, height;

private:

	Shader* deferredShader;
	GLuint quadVAO, quadVBO;
	int viewportTexture;
	bool wireframe;

	void InitG_Buffer();
	void InitDeferredShadingBuffer();

	void SetupQuad();
};