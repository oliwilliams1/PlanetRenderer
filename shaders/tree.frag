#version 430 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec3 FragPos;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_View;
    vec3 cameraPos;
    float deltaTime;
};

void main() {
	gPosition = FragPos;
	if (distance(FragPos, cameraPos) > 1450.0) discard;
	
	gAlbedo = vec4(1.0, 0.0, 0.0, 1.0);
	gNormal = vec3(1.0, 0.0, 0.0);
}