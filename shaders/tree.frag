#version 430 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec3 FragPos;
in vec2 UV;
in flat int imposterRow;
in flat mat3 TBN;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_View;
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
    vec2 sampledUV = imposterOffset + uv * imposterSize;

    return sampledUV;
}

void main() {
    vec2 uv = imposterSampler(UV, imposterRow, 0);
    vec4 albedo = texture(u_Albedo, uv);
    if (albedo.a < 0.5) discard;

    vec3 normal = texture(u_Normal, uv).rgb * 2.0 - 1.0;
    normal = normalize(TBN * normalize(normal));

	gPosition = FragPos;
	gNormal = normal;
    gAlbedo = vec4(albedo.rgb, 1.0);
}