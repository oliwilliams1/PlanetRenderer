#version 430 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec3 FragPos;
in vec3 Normal;
in flat mat3 TBN;
in vec2 UV;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_View;
    vec3 cameraPos;
    float deltaTime;
};

uniform sampler2D u_Albedo;
uniform sampler2D u_Normal;

void main() {
	gPosition = FragPos;
	
	vec4 albedo = texture(u_Albedo, UV);
	if (albedo.a == 0.0) discard;

	gAlbedo = vec4(albedo.rgb, 1.0);
	gPosition = FragPos;
	gNormal = TBN * texture(u_Normal, UV).xyz;
}