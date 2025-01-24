#include "app.h"

static bool dragging = false;
static double offsetX, offsetY;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			dragging = true;
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			offsetX = xpos;
			offsetY = ypos;
		}
		else if (action == GLFW_RELEASE) {
			dragging = false;
		}
	}
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	if (dragging) {
		int width, height;
		glfwGetWindowSize(window, &width, &height);
		int x, y;
		glfwGetWindowPos(window, &x, &y);

		if (ypos > 32) return;
		if (xpos < 175 || xpos > width - 175) return;
		// Calculate new window position
		int newX = x + static_cast<int>(xpos - offsetX);
		int newY = y + static_cast<int>(ypos - offsetY);

		glfwSetWindowPos(window, newX, newY);
	}
}

App::App() {
	windowWidth = 1600;
	windowHeight = 900;

	viewportWidth = 1200;
	viewportHeight = 675;
	imposterRenderingWindowOpen = false;

	currentFPS = 0;
	frameCount = 0;
	deltaTime = 0.0f;
	lastTime = glfwGetTime();
	lastDeltaTime = glfwGetTime();

	InitWindow();
	InitOpenGLParams();
	InitImGui();

	AssetManager::System::Init();
	Sable::Console::Init();

	CamInitData camInitData{};
	camInitData.fov = 70.0f;
	camInitData.pitch = 0.0f;
	camInitData.yaw = -220.0f;
	camInitData.speed = 200.0f;
	camInitData.position = glm::vec3(1700.0f, 0.0f, -1200.0f);

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

	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

	// Create a window with OpenGL
	window = glfwCreateWindow(windowWidth, windowHeight, "Planet Renderer", nullptr, nullptr);
	if (!window) {
		std::cout << stderr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	// Calculate the center position
	int windowWidth, windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	int posX = (mode->width - windowWidth) / 2;
	int posY = (mode->height - windowHeight) / 2;

	// Set the window position to the center
	glfwSetWindowPos(window, posX, posY);

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	glfwMakeContextCurrent(window);

	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);

	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cout << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
		return;
	}

	Sable::Console::Log("OpenGL version: %s", glGetString(GL_VERSION));
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
	SetupImGuiStyle();
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

		mainPlanet->atmosphere->Draw();
		
		// Setup ImGui for new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Top Bar", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
		ImGui::SetWindowSize(ImVec2(windowWidth, 32));
		ImGui::SetWindowPos(ImVec2(0, 0));

		ImGui::Text("FPS: %i", (int)currentFPS);

		// Center the title
		ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f - ImGui::CalcTextSize("Sable Engine").x * 0.5f);
		ImGui::Text("Sable Engine");

		// Minimize and close buttons
		ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize("x").x - ImGui::CalcTextSize("-").x * 6);
		if (ImGui::Button("-")) {
			glfwIconifyWindow(window);
		}
		ImGui::SameLine();
		if (ImGui::Button("x")) {
			glfwSetWindowShouldClose(window, true);
		}

		ImGui::End();

		ImGui::Begin("Engine", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
		ImGui::SetWindowSize(ImVec2(windowWidth, windowHeight - 32));
		ImGui::SetWindowPos(ImVec2(0, 32));

		ImGui::Columns(2, "MainGridLayout");
		ImGui::SetColumnWidth(0, viewportWidth + 16);

		deferredRenderer->DisplayViewportImGui();

		if (ImGui::BeginTabBar("Bottom Tabs")) {
			if (ImGui::BeginTabItem("Console")) {
				Sable::Console::DisplayConsoleImGui();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Asset Manager")) {
				AssetManager::System::GetInstance().DisplayGui();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}


		ImGui::NextColumn();

		if (ImGui::BeginTabBar("Right Tabs")) {
			if (ImGui::BeginTabItem("Deferred Renderer")) {
				deferredRenderer->DebugDraw();
				if (ImGui::Checkbox("Wireframe", &wireframe)) {
					if (wireframe) {
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					}
					else {
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					}
				}
				ImGui::Checkbox("Open/Close imposter rendering menu", &imposterRenderingWindowOpen);
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Camera")) {
				camera->DebugDraw();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Main Planet")) {
				mainPlanet->DebugDraw();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Atmosphere")) {
				mainPlanet->atmosphere->DebugDraw();
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

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
	Sable::Console::Shutdown();
	AssetManager::System::Shutdown();
	delete deferredRenderer;
	delete camera;
	delete mainPlanet;
	delete imposterRenderer;
	delete planetShader;
	glfwDestroyWindow(window);
	glfwTerminate();
}