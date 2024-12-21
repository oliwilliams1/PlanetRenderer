#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "utils.h"

class Noise {
public:
	GLuint* cubemapNoiseTexture;
	GLuint* cubemapNormalTexture;
	Noise(int cubemapResolution, GLuint* cubemapNoiseTexture, GLuint* cubemapNormalTexture);
	~Noise();
	void Dispatch(int seed);
	void DebugDraw();
	int cubemapResolution;
	GLuint cubemapNoiseShaderProgram, cubemapNormalShaderProgram;

private:
	float sampleOffsetSize;

	bool needToDispatch;
	float lastDispatchTime;
	int seed;

	GLuint fboCubemapNoise, fboCubemapNormal;
	GLuint noise_seedLocation;
	GLuint normal_NoiseSamplerLocation, normal_SampleOffset;

	void CreateTextures();
	void CreateFramebuffers();
};