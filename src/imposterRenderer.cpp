#include "imposterRenderer.h"

ImposterRenderer::ImposterRenderer(App* app, GLuint UBO) {
	this->app = app;
	this->ortho = true;
	this->orthoScale = 64.0f;

	this->resolution = 1024.0f;

	deferredRenderer = new DeferredRenderer(resolution, resolution);

	CamInitData camInitData{};
	camInitData.fov = 70.0f;
	camInitData.pitch = 0.0f;
	camInitData.yaw = 90.0f;
	camInitData.speed = 100.0f;
	camInitData.position = glm::vec3(0.0f, 0.0f, -5.0f);

	camera = new Camera(app->window, &app->deltaTime, 512, 512, camInitData, UBO);
	imposterShader = new Shader("shaders/imposter.vert", "shaders/imposter.frag", "Imposter Shader");
	imposterObject = new ImposterObject(imposterShader, &orthoScale);
}

void ImposterRenderer::Render() {
	if (ortho) {
		glm::mat4 m_Proj = glm::ortho(-orthoScale, orthoScale, -orthoScale, orthoScale, -orthoScale * 10.0f, orthoScale * 10.0f);
		glm::mat4 m_View = glm::lookAt(glm::vec3(0.0f, 50.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		camera->cameraData.m_ProjView = m_Proj * m_View;
		camera->UpdateBuffers();
	}
	else {
		camera->update(app->mouseDelta);
	}

	deferredRenderer->Bind();
	imposterShader->use();
	imposterObject->Draw();
	deferredRenderer->Render();    
}

void ImposterRenderer::DebugDraw() {
	ImGui::Begin("Imposter Renderer");

	ImGui::SetWindowSize(ImVec2(934, 596));
	ImGui::Columns(2, "GridLayout");
	ImGui::SetColumnWidth(0, 544);

	deferredRenderer->DisplayViewportImGui(glm::vec2(512.0f));

	ImGui::NextColumn();
	deferredRenderer->DebugDraw();
	imposterObject->DebugDraw();
	ImGui::Checkbox("Preview ortho camera", &ortho);

	ImGui::Columns(1);
	ImGui::End();
}

ImposterObject::ImposterObject(Shader* shader, float* orthoScale) {
	this->shader = shader;
	this->orthoScale = orthoScale;
	this->pos = glm::vec3(0.0f, -8.0f, 0.0f);
	this->rot = glm::vec3(0.0f, 0.0f, 0.0f);
	this->scale = 0.023f;

	objData = LoadObject("tree0");
	GenerateInstanceData();

	SetupBuffers();
}

void ImposterObject::GenerateInstanceData() {
	instanceData.clear();

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {

			glm::mat4 translation = glm::translate(glm::mat4(1.0f),
				glm::vec3(-64.0f + x * 16.0f + 8.0f,
					0.0f,
					-64.0f + y * 16.0f + 8.0f));

			glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(x * 45.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(y * 22.5f), glm::vec3(1.0f, 0.0f, 0.0f));

			glm::mat4 m_Model = translation * rotX * rotY;
			instanceData.push_back(m_Model);
		}
	}
}

void ImposterObject::UpdateModelMatrix() {
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, pos);

	model = glm::rotate(model, glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));

	model = glm::scale(model, glm::vec3(scale));

	shader->use();
	glUniformMatrix4fv(m_MasterModelLocation, 1, GL_FALSE, glm::value_ptr(model));
}

void ImposterObject::SetupBuffers() {
	m_MasterModelLocation = glGetUniformLocation(shader->shaderProgram, "m_ModelMaster");

	UpdateModelMatrix();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, objData.vertices.size() * sizeof(glm::vec3), objData.vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &NBO);
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, objData.normals.size() * sizeof(glm::vec3), objData.normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, objData.indices.size() * sizeof(unsigned int), objData.indices.data(), GL_STATIC_DRAW);

	indicesCount = objData.indices.size();

	glGenBuffers(1, &instanceBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
	glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(glm::mat4), instanceData.data(), GL_STATIC_DRAW);
	for (int i = 0; i < 4; i++) {
		glEnableVertexAttribArray(i + 2);
		glVertexAttribPointer(i + 2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
		glVertexAttribDivisor(i + 2, 1);
	}
	glBindVertexArray(0);
}

ObjectData ImposterObject::LoadObject(std::string objName) {
	std::ifstream bobjFile("resources/trees/" + objName + ".bobj");
	std::ifstream objFile("resources/trees/" + objName + ".obj");

	std::vector<ObjectData> objData;
	if (bobjFile.good()) {
		std::cout << "Loading BOBJ file: " << objName << std::endl;
		std::vector<ObjectData> objects = DeserializeObjects("resources/trees/" + objName + ".bobj");
		return objects[0];
	}
	else if (objFile.good()) {
		std::cout << "Loading OBJ file: " << objName << std::endl;
		ModifyBrokenOBJFile("resources/trees/" + objName + ".obj");
		LoadAdvancedModel("resources/trees/" + objName + ".obj", objData);
		SerializeObjects(objData, "resources/trees/" + objName + ".bobj");
		std::cout << objData.size() << std::endl;
		return objData[0];
	}
	else {
		std::cerr << "Failed to load object: " << objName << std::endl;
	}
}

void ImposterObject::ModifyBrokenOBJFile(std::string path) {
	std::string inputData;
	std::string outputData;

	if (!ReadFile(path.c_str(), inputData)) {
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

	if (!WriteFile(path.c_str(), outputData)) {
		std::cerr << "Failed to write file to fix OBJ issue: " << path << std::endl;
	}
}

void ImposterObject::Draw() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElementsInstanced(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0, instanceData.size());
	glBindVertexArray(0);
}

void ImposterObject::DebugDraw() {
	if (ImGui::DragFloat3("Position", &pos.x, 1.0f, 0.0f, 0.0f, "%.1f"))      UpdateModelMatrix();
	if (ImGui::DragFloat3("Rotation", &rot.x, 1.0f, -180.0f, 180.0f, "%.1f")) UpdateModelMatrix();
	if (ImGui::DragFloat ("Scale",    &scale, 0.001f, 0.001f, 0.0f, "%.3f"))  UpdateModelMatrix();
}

ImposterObject::~ImposterObject() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &IBO);
	glDeleteBuffers(1, &instanceBuffer);
}