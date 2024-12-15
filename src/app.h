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
#include "camera.h"
#include "planet.h"
#include "noise.h"

// Forward declarations
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

    Noise* noise;
    Camera* camera;
    PlanetShader* planetShader;
    Planet* mainPlanet;

    GLuint gBuffer, gPosition, gNormal, gAlbedo;

private:
    void InitWindow();
    void InitOpenGLParams();
    void InitImGui();
    void InitMouseEvents();
    void InitG_Buffer();

    static void OnWindowResize(GLFWwindow* window, int width, int height);

    glm::dvec2 mousePos, mouseDelta;
    bool wireframe;
};