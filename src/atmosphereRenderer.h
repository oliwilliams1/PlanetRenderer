#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/common.hpp>
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
	GLuint gRendered, gFragPos;
	
	Shader* shader;
	GLuint gAtmosphereFBO;

	int u_Steps;
	float u_MinAtmsDistance, u_MaxAtmsDistance, u_AtmsExpFalloff, u_FalloffB;
	glm::vec3 u_ColourContribs;
	glm::vec3 u_Colour1, u_Colour2, u_Colour3;
	glm::vec3 u_SunColour;
	float u_SunIntensity;

	GLuint quadVAO, quadVBO;
	float width, height;
};