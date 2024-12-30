#version 430 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedo;

in vec3 FragPos;
in vec2 UV;
in flat mat3 TBN;

in flat float LowerRow;
in flat int Col;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_CameraRotation;
    vec3 cameraPos;
    float deltaTime;
};

uniform sampler2D u_Albedo;
uniform sampler2D u_Normal;

vec2 imposterSampler(vec2 uv, int row, int col) {
	const int gridSize = 8;
	row = gridSize - row - 1;

	vec2 imposterSize = vec2(1.0 / gridSize);
	vec2 imposterOffset = vec2(col, row) * imposterSize;
	return imposterOffset + uv * imposterSize;
}

void main() {
	int lowerRow = int(LowerRow);
    int upperRow = min(lowerRow + 1, 8 - 1);
	float RowBlendFactor = LowerRow - lowerRow;

	vec2 uvLower = imposterSampler(UV, lowerRow, Col);
	vec4 albedoLower = texture(u_Albedo, uvLower);
	vec3 normalLower = texture(u_Normal, uvLower).rgb * 2.0 - 1.0;

	vec2 uvUpper = imposterSampler(UV, upperRow, Col);
	vec4 albedoUpper = texture(u_Albedo, uvUpper);
	vec3 normalUpper = texture(u_Normal, uvUpper).rgb * 2.0 - 1.0;

	vec4 finalAlbedo = mix(albedoLower, albedoUpper, RowBlendFactor);
	vec3 finalNormal = normalize(TBN * normalize(mix(normalLower, normalUpper, RowBlendFactor)));

	if (finalAlbedo.r < 0.1) discard;

	gPosition = FragPos;
	gNormal = finalNormal;
	gAlbedo = vec4(finalAlbedo.rgb, 1.0);
}