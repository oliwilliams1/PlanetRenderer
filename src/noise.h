#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "utils.h"

class Noise {
public:
	GLuint noiseTexture, normalTexture;
	Noise();
	~Noise();
	void Dispatch();
	void DebugDraw();

private:
	int seed, octaves;
	float scale, persistence;

	bool needToDispatch;
	float lastDispatchTime;

	GLuint noiseShaderProgram, normalShaderProgram, fboNoise, fboNormal;
	GLuint seedLocation, octavesLocation, scaleLocation, persistenceLocation, normal_TerrainHeightmapLocation;

	GLuint CompileComputeShader(const char* source);
	void CreateTextures();
	void CreateFramebuffers();
};