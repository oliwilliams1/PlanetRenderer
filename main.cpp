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

	// Init ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Init objects
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	Camera camera = Camera(window, width, height);
	Shader mainShader = Shader("shaders/triangle.vert", "shaders/triangle.frag", "Cube Shader");
	Object triangle = Object();

	glm::dvec2 mousePos{0.0, 0.0};
	glm::dvec2 mouseDelta{0.0, 0.0};

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
		glClear(GL_COLOR_BUFFER_BIT);

		// Setup ImGui for new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Extremely simple shader reloader (works with one shader)
		ImGui::Begin("Shader Reloader");
		ImGui::Text("Reload following shaders...");
		if (ImGui::Button(mainShader.shaderName)) {
			Shader newShader(mainShader.vsSource.c_str(), mainShader.fsSource.c_str(), mainShader.shaderName);
		};
		ImGui::End();
		
		// Render objects
		triangle.Draw();

		// ImGui debug windows
		camera.debugDraw();

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