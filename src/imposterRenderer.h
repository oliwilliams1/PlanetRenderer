#pragma once
#include <sstream>
#include <string>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "app.h"
#include "deferredRenderer.h"
#include "camera.h"
#include "object.h"
#include "objectSerializer.h"

class DeferredRenderer;
class Camera;
class App;
class ImposterObject;

class ImposterRenderer {
public:
	ImposterRenderer(App* app, GLuint UBO);

	void Render();
	void DebugDraw();
	
private:
	bool ortho;
	float orthoScale;

	App* app;
	Camera* camera;
	DeferredRenderer* deferredRenderer;
	ImposterObject* imposterObject;
	Shader* imposterShader;
};

class ImposterObject {
public:
	ImposterObject(Shader* shader, float* orthoScale);
	~ImposterObject();
	ObjectData LoadObject(std::string objName);
	void Draw();
	void DebugDraw();

private:
	Shader* shader;
	ObjectData objData;

	void SetupBuffers();
	std::vector<glm::mat4> instanceData;
	GLuint VAO, VBO, NBO, IBO, instanceBuffer;

	void UpdateModelMatrix();
	GLuint m_MasterModelLocation;
	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scale;

	int indicesCount;
	float* orthoScale;
	void ModifyBrokenOBJFile(std::string path);
};