#pragma once
#include <sstream>
#include <string>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "app.h"
#include "deferredRenderer.h"
#include "camera.h"
#include "object.h"

class DeferredRenderer;
class Camera;
class App;
class ImposterObject;

class ImposterRenderer {
public:
	ImposterRenderer(App* app, GLuint UBO);
	~ImposterRenderer();

	void Render();
	void DebugDraw();
	
private:
	App* app;
	Camera* camera;
	DeferredRenderer* deferredRenderer;
	ImposterObject* imposterObject;
	Shader* imposterShader;
};

class ImposterObject : public Object {
public:
	ImposterObject(Shader* shader);

private:
	void ModifyBrokenOBJFile(const char* path);
};