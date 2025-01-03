#pragma once

#include "app.h"

class App;
class Shader;

class DeferredRenderer {
public:
	DeferredRenderer(int width, int height);
	~DeferredRenderer();

	void Bind();
	void DebugDraw();
	void DisplayViewportImGui();
	void Render();

private:
	int width, height;
	GLuint gBuffer, gPosition, gNormal, gAlbedo, gDepth;

	Shader* deferredShader;
	GLuint fboShading, mainTexture;
	GLuint quadVAO, quadVBO;
	GLuint gPositionLocation, gNormalLocation, gAlbedoLocation;
	int viewportTexture;
	bool wireframe;

	void InitG_Buffer();
	void InitDeferredShadingBuffer();
	void InitDeferredShadingUniforms();

	void SetupQuad();
};