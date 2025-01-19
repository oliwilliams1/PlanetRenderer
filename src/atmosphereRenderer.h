#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "shader.h"

class Atmosphere {
public:
	GLuint uPlanetCentre, uPlanetRadius;

	Atmosphere(float width, float height, GLuint gFragPos, GLuint gDepth);
	~Atmosphere();

	void Draw();
	void DebugDraw();
	void CreateTextures();
	void SetupQuad();

private:
	GLuint gDepth, gFragPos;
	
	Shader* shader;
	GLuint gAtmosphereFBO, gAtmosphere;
	
	GLuint quadVAO, quadVBO;
	float width, height;
};