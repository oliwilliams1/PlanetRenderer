#version 430 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec3 FragPos;
in vec3 Normal;
in vec2 UV;

void main() {
	gPosition = FragPos;
	gNormal = Normal;
	gAlbedo = vec4(UV.x, 0.0, UV.y, 1.0);
}