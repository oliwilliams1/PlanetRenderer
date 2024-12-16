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
#include "deferredRenderer.h"
#include "camera.h"
#include "planet.h"
#include "noise.h"

// Forward declarations
class DeferredRenderer;
class PlanetShader;
class Planet;
class Camera;

class App {
public:
    App();
    void Mainloop();
    ~App();

    int width, height;
    GLFWwindow* window;

    DeferredRenderer* deferredRenderer;
    Noise* noise;
    Camera* camera;
    PlanetShader* planetShader;
    Planet* mainPlanet;

private:
    void InitWindow();
    void InitOpenGLParams();
    void InitImGui();
    void InitMouseEvents();

    glm::dvec2 mousePos, mouseDelta;
    bool wireframe;
};