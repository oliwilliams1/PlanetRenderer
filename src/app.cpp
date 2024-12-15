#include "app.h"

App::App() {
    width = 1600;
    height = 900;
    
    InitWindow();
    InitOpenGLParams();
    InitImGui();
	InitG_Buffer();

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
    // Set a pointer to camera for OnWindowResize to call from it
	glfwSetWindowUserPointer(window, &camera);
	glfwSetFramebufferSizeCallback(window, OnWindowResize);

	mousePos   = glm::dvec2(0.0, 0.0);
	mouseDelta = glm::dvec2(0.0, 0.0);

	glfwGetCursorPos(window, &mousePos.x, &mousePos.y);
}

// Static function for window resizing
void App::OnWindowResize(GLFWwindow* window, int width, int height) {
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	camera->OnWindowResize(window, width, height); // Changes aspect ratio in camera
}

void App::InitG_Buffer() {
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

		// Bind gBuffer
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedo);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render objects
		planetShader->use();
		mainPlanet->Draw();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Setup ImGui for new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// ImGui debug windows
		camera->DebugDraw();
		mainPlanet->DebugDraw();
		noise->DebugDraw();

		// Fast debug window
		ImGui::Begin("Settings");
		ImGui::Checkbox("Wireframe", &wireframe);
		if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		ImGui::Image((ImTextureID)(intptr_t)gPosition, ImVec2(width / 5.0f, height / 5.0f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::Image((ImTextureID)(intptr_t)gNormal,   ImVec2(width / 5.0f, height / 5.0f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		ImGui::Image((ImTextureID)(intptr_t)gAlbedo,   ImVec2(width / 5.0f, height / 5.0f), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
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