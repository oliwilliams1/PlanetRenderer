#include "PlanetRenderer.h"

int main()
{
	// Init GLFW
	if (!glfwInit()) {
		std::cout << stderr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// Create a window with OpenGL
	GLFWwindow* window = glfwCreateWindow(800, 600, "Planet Renderer", nullptr, nullptr);
	if (!window) {
		std::cout << stderr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
		return -1;
	}

	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	Shader triangleShader = Shader("shaders/triangle.vert", "shaders/triangle.frag");
	Object triangle = Object();

	while (!glfwWindowShouldClose(window)) {
		triangle.Draw();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}