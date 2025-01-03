#include "app.h"

App::App() {
	windowWidth = 1600;
	windowHeight = 900;

	viewportWidth = 1200;
	viewportHeight = 675;
	imposterRenderingWindowOpen = true;

	currentFPS = 0;
	frameCount = 0;
	deltaTime = 0.0f;
	lastTime = glfwGetTime();
	lastDeltaTime = glfwGetTime();

	InitWindow();
	InitOpenGLParams();
	InitImGui();

	CamInitData camInitData{};
	camInitData.fov = 70.0f;
	camInitData.pitch = -30.0f;
	camInitData.yaw = -145.0f;
	camInitData.speed = 300.0f;
	camInitData.position = glm::vec3(400.0f, 1000.0f, 450.0f);

	deferredRenderer = new DeferredRenderer(viewportWidth, viewportHeight);
	camera = new Camera(window, &deltaTime, viewportWidth, viewportHeight, camInitData);
	planetShader = new PlanetShader("shaders/planet.vert", "shaders/planet.frag", "Planet Shader");
	mainPlanet = new Planet(this, planetShader);
	imposterRenderer = new ImposterRenderer(this, camera->UBO);
	
	InitMouseEvents();
}

void App::InitWindow() {
	if (!glfwInit()) {
		std::cout << stderr << "Failed to initialize GLFW" << std::endl;
		return;
	}

	// Create a window with OpenGL
	window = glfwCreateWindow(windowWidth, windowHeight, "Planet Renderer", nullptr, nullptr);
	if (!window) {
		std::cout << stderr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

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
		frameCount++;
		float currentTime = glfwGetTime();
		if (currentTime - lastTime >= 1.0f) {
			glfwSetWindowTitle(window, ("Planet Renderer - FPS: " + std::to_string(frameCount)).c_str());
			currentFPS = frameCount;
			frameCount = 0;
			lastTime = currentTime;
		}

		// Delta time calc
		deltaTime = currentTime - lastDeltaTime;
		lastDeltaTime = currentTime;

		// Pre-frame stuff
		glfwPollEvents();

		// Calculate mouse delta
		glm::dvec2 currentMousePos(0);
		glfwGetCursorPos(window, &currentMousePos.x, &currentMousePos.y);

		mouseDelta = currentMousePos - mousePos;
		mousePos   = currentMousePos;

		if (imposterRenderingWindowOpen) { 
			camera->UpdateBuffers(); // Update buffers, but not camera pos
		}
		else {
			camera->update(mouseDelta);
		}

		// Clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		deferredRenderer->Bind();

		// Render objects
		planetShader->use();
		mainPlanet->Draw();
		deferredRenderer->Render();

		if (imposterRenderingWindowOpen) {
			imposterRenderer->Render();
		}

		// Setup ImGui for new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Engine", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
		ImGui::SetWindowSize(ImVec2(windowWidth, windowHeight));
		ImGui::SetWindowPos(ImVec2(0, 0));

		ImGui::Columns(2, "MainGridLayout");
		ImGui::SetColumnWidth(0, viewportWidth + 16);

		deferredRenderer->DisplayViewportImGui();

		ImGui::NextColumn();

		deferredRenderer->DebugDraw();
		ImGui::Separator();
		camera->DebugDraw();
		ImGui::Separator();
		mainPlanet->DebugDraw();
		ImGui::Separator();

		ImGui::Checkbox("Open/Close imposter rendering menu", &imposterRenderingWindowOpen);
		ImGui::End();

		if (imposterRenderingWindowOpen) imposterRenderer->DebugDraw();

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