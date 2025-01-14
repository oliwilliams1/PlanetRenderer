#version 430 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec3 FragPos;
in vec3 Normal;
in vec2 UV;

uniform sampler2D albedoMap;

void main() {
	vec4 colour = texture(albedoMap, UV);

	vec3 normal = Normal;

	normal.z *= -1.0;

	gPosition = FragPos;
	gNormal = normal * 0.5 + 0.5;
	gAlbedo = vec4(colour.rgb, 1.0);
}