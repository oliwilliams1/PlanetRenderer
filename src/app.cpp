#include "app.h"

App::App() {
    width = 1600;
    height = 900;
    
    InitWindow();
    InitOpenGLParams();
    InitImGui();

	deferredRenderer = new DeferredRenderer(this);
    noise = new Noise();
    camera = new Camera(this);
    planetShader = new PlanetShader("shaders/planet.vert", "shaders/planet.frag", "Planet Shader");
    mainPlanet = new Planet(this, planetShader);
    
    InitMouseEvents();
}

void App::InitWindow() {
    if (!glfwInit()) {
		std::cout << stderr << "Failed to initialize GLFW" << std::endl;
		return;
	}

	// Create a window with OpenGL
	window = glfwCreateWindow(width, height, "Planet Renderer", nullptr, nullptr);
	if (!window) {
		std::cout << stderr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
		return;
	}

	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
}

void App::InitOpenGLParams() {
    // Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

	// Define patch vertex count
	glPatchParameteri(GL_PATCH_VERTICES, 4);

    wireframe = false;
}

void App::InitImGui() {
    // Init ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void App::InitMouseEvents() {
	mousePos   = glm::dvec2(0.0, 0.0);
	mouseDelta = glm::dvec2(0.0, 0.0);

	glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
}

void App::Mainloop() {
    while (!glfwWindowShouldClose(window)) {
        // Pre-frame stuff
		glfwPollEvents();

		// Calculate mouse delta
		glm::dvec2 currentMousePos(0);
		glfwGetCursorPos(window, &currentMousePos.x, &currentMousePos.y);

		mouseDelta = currentMousePos - mousePos;
		mousePos   = currentMousePos;
		camera->update(mouseDelta);

		// Clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		deferredRenderer->Bind();

		// Render objects
		planetShader->use();
		mainPlanet->Draw();

		deferredRenderer->Render();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Setup ImGui for new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// ImGui debug windows
		camera->DebugDraw();
		mainPlanet->DebugDraw();
		noise->DebugDraw();
		deferredRenderer->DebugDraw();
		deferredRenderer->DisplayViewportImGui();

		// Fast debug window
		ImGui::Begin("Settings");
		ImGui::Checkbox("Wireframe", &wireframe);
		if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		ImGui::End();

		// Render ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap buffers
		glfwSwapBuffers(window);
    }
}

App::~App() {
    glfwDestroyWindow(window);
    glfwTerminate();
}