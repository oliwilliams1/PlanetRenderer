#pragma once

#include "app.h"

class App;
class Shader;

class DeferredRenderer {
public:
	DeferredRenderer(App* app);
	~DeferredRenderer();

	void Bind();
	void DebugDraw();
	void DisplayViewportImGui();
	void Render();

private:
	App* app;
	GLuint gBuffer, gPosition, gNormal, gAlbedo;

	Shader* deferredShader;
	GLuint fboShading, mainTexture;
	GLuint quadVAO, quadVBO;
	GLuint gPositionLocation, gNormalLocation, gAlbedoLocation;

	void InitG_Buffer();
	void InitDeferredShadingBuffer();
	void InitDeferredShadingUniforms();

	void SetupQuad();
};