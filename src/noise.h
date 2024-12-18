#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "utils.h"

class Noise {
public:
	GLuint cubemapNoiseTexture, cubemapNormalTexture;
	Noise();
	~Noise();
	void Dispatch();
	void DebugDraw();
	int cubemapResolution = 512;

private:
	float sampleOffsetSize;

	bool needToDispatch;
	float lastDispatchTime;

	GLuint cubemapNoiseShaderProgram, cubemapNormalShaderProgram;
	GLuint fboCubemapNoise, fboCubemapNormal;
	GLuint normal_NoiseSamplerLocation, normal_SampleOffset;

	void CreateTextures();
	void CreateFramebuffers();
};