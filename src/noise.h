#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "utils.h"

class Noise {
public:
	GLuint noiseTexture, normalTexture, cubemapNoiseTexture;
	Noise();
	~Noise();
	void Dispatch();
	void DebugDraw();

private:
	float sampleOffsetSize;

	bool needToDispatch;
	float lastDispatchTime;

	GLuint cubemapNoiseShaderProgram, fboCubemapNoise;

	GLuint CompileComputeShader(const char* source);
	void CreateTextures();
	void CreateFramebuffers();
};