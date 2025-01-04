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

    deferredRenderer->DisplayViewportImGui();

    ImGui::NextColumn();
    deferredRenderer->DebugDraw();
    imposterObject->DebugDraw();

    ImGui::Columns(1);
    ImGui::End();
}

ImposterObject::ImposterObject(Shader* shader) : Object(shader) {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
    
	ModifyBrokenOBJFile("resources/trees/tree0.obj");
    LoadAdvancedModel("resources/trees/tree0.obj", vertices, normals, indices);

	SetData(vertices, normals, indices);
}

void ImposterObject::ModifyBrokenOBJFile(const char* path) {
    std::string inputData;
    std::string outputData;

    if (!ReadFile(path, inputData)) {
        std::cerr << "Failed to read file to fix OBJ issue: " << path << std::endl;
        return;
    }

    std::istringstream stream(inputData);
    std::string line;

    while (std::getline(stream, line)) {
        if (line.substr(0, 2) == "v ") {
            std::istringstream vertexStream(line);
            std::string vertexPrefix;
            double x, y, z;

            vertexStream >> vertexPrefix >> x >> y >> z;
			outputData += "v " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z) + "\n";
        }
        else {
			outputData += line + "\n";
        }
    }

    if (!WriteFile(path, outputData)) {
        std::cerr << "Failed to write file to fix OBJ issue: " << path << std::endl;
    }
}