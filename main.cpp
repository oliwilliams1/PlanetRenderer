#include "PlanetRenderer.h"

int main()
{
	int width = 800;
	int height = 600;
	// Init GLFW
	if (!glfwInit()) {
		std::cout << stderr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// Create a window with OpenGL
	GLFWwindow* window = glfwCreateWindow(width, height, "Planet Renderer", nullptr, nullptr);
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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glFrontFace(GL_CCW);

	// Init ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Init objects
	Camera camera = Camera(window, width, height);
	PlanetShader planetShader = PlanetShader("shaders/planet.vert", "shaders/planet.frag", "Planet Shader");
	Planet mainPlanet = Planet(&planetShader);

	glm::dvec2 mousePos   = glm::dvec2(0.0, 0.0);
	glm::dvec2 mouseDelta = glm::dvec2(0.0, 0.0);

	glfwGetCursorPos(window, &mousePos.x, &mousePos.y);

	while (!glfwWindowShouldClose(window)) {
		// Pre-frame stuff
		glfwPollEvents();

		glm::dvec2 currentMousePos;
		glfwGetCursorPos(window, &currentMousePos.x, &currentMousePos.y);

		mouseDelta = currentMousePos - mousePos;
		mousePos = currentMousePos;

		camera.update(mouseDelta);

		// Clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Setup ImGui for new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		// Render objects
		mainPlanet.Draw();

		// ImGui debug windows
		camera.debugDraw();
		mainPlanet.ObjectDebugImGUI();

		// Render ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap buffers
		glfwSwapBuffers(window);
	}

	// Cleanup
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}