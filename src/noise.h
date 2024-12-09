#pragma once

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "utils.h"

class Noise {
public:
	GLuint noiseTexture;
	Noise();
	~Noise();
	void Dispatch();
	void DebugDraw();

private:
	GLuint shaderProgram, fbo;
	void CreateTexture();
	void CreateFramebuffer();
};