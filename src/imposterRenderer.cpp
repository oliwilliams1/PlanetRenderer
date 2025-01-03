#include "imposterRenderer.h"

ImposterRenderer::ImposterRenderer(App* app, GLuint UBO) {
    this->app = app;
    deferredRenderer = new DeferredRenderer(512, 512);

    CamInitData camInitData{};
    camInitData.fov = 70.0f;
    camInitData.pitch = 0.0f;
    camInitData.yaw = 90.0f;
    camInitData.speed = 10.0f;
    camInitData.position = glm::vec3(0.0f, 0.0f, -5.0f);

    camera = new Camera(app->window, &app->deltaTime, 512, 512, camInitData, UBO);
    imposterShader = new Shader("shaders/default.vert", "shaders/default.frag", "Imposter Shader");
    imposterObject = new ImposterObject(imposterShader);
}

ImposterRenderer::~ImposterRenderer() {}

void ImposterRenderer::Render() {
    camera->update(app->mouseDelta);

    deferredRenderer->Bind();
    imposterShader->use();
    imposterObject->Draw();
    deferredRenderer->Render();    
}

void ImposterRenderer::DebugDraw() {
    ImGui::Begin("Imposter Renderer");

    ImGui::SetWindowSize(ImVec2(938, 596));
    ImGui::Columns(2, "GridLayout");
    ImGui::SetColumnWidth(0, 544);

    ImGui::BeginChild("Viewport", ImVec2(0, 0), true);
    deferredRenderer->DisplayViewportImGui();
    ImGui::SameLine();

    ImGui::EndChild();

    ImGui::NextColumn();
    ImGui::BeginChild("G-Buffer", ImVec2(0, 0), true);
    deferredRenderer->DebugDraw();
    ImGui::EndChild();

    ImGui::Columns(1);
    ImGui::End();
}

ImposterObject::ImposterObject(Shader* shader) : Object(shader) {
    std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
    LoadModel("resources/icosphere.obj", vertices, indices);

	SetData(vertices, std::vector<glm::vec3>(0), indices);
}