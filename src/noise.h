#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "utils.h"

class Noise {
public:
	GLuint noiseTexture;
	Noise();
	~Noise();
	void Dispatch();
	void DebugDraw();

private:
	int seed, octaves;
	float scale, persistence;

	bool needToDispatch;
	float lastDispatchTime;

	GLuint shaderProgram, fbo;
	GLuint seedLocation, octavesLocation, scaleLocation, persistenceLocation;
	void CreateTexture();
	void CreateFramebuffer();
};