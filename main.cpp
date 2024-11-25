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

	// Initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	Camera camera = Camera(window, width, height);
	Shader triangleShader = Shader("shaders/triangle.vert", "shaders/triangle.frag");
	Object triangle = Object();

	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		camera.update();
		triangle.Draw();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}