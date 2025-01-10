#pragma once


// Standard library stuff
#include <iostream>

// External libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Own stuff
#include "assetManager.h"
#include "imposterRenderer.h"
#include "deferredRenderer.h"
#include "camera.h"
#include "planet.h"
#include "noise.h"

// Forward declarations
class ImposterRenderer;
class DeferredRenderer;
class PlanetShader;
class Planet;

class App {
public:
	App();
	void Mainloop();
	~App();
	
	GLFWwindow* window;
	int viewportWidth, viewportHeight;

	DeferredRenderer* deferredRenderer;
	Camera* camera;
	PlanetShader* planetShader;
	Planet* mainPlanet;

	float deltaTime;
	glm::dvec2 mousePos, mouseDelta;

private:
	int frameCount;
	float lastTime, lastDeltaTime, currentFPS;
	int windowWidth, windowHeight;
	void InitWindow();
	void InitOpenGLParams();
	void InitImGui();
	void InitMouseEvents();

	bool wireframe;

	ImposterRenderer* imposterRenderer;
	bool imposterRenderingWindowOpen;
};