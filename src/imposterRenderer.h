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
	bool needToRender;
	bool ortho;
	float orthoScale;
	float resolution;

	App* app;
	Camera* camera;
	DeferredRenderer* deferredRenderer;
	ImposterObject* imposterObject;
	Shader* imposterShader;

	Shader* gridShader;
	Object* grid;

	char saveToFileBuffer[256];
};

struct ObjectBuffer {
	GLuint albedoMap;
	GLuint VAO, VBO, NBO, UVBO, IBO, instanceBuffer;
	int indicesCount;
};

class ImposterObject {
public:
	ImposterObject(Shader* shader, const char* name);
	~ImposterObject();
	void Draw();
	void DebugDraw();

private:
	Shader* shader;

	std::vector<ObjectData> LoadObject(std::string objName);
	
	void SetupBuffers(const ObjectData& objData);
	std::vector<glm::mat4> instanceData;
	std::vector<ObjectBuffer> objBuffers;

	void GenerateInstanceData();
	void UpdateModelMatrix();
	GLuint m_MasterModelLocation;
	GLuint albedoMapLocation;
	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scale;
	float overallScale;

	void ModifyBrokenOBJFile(std::string path);
};