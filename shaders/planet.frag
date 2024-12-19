#version 410 core

in vec3 Normal;
in vec3 FragPos;
in vec2 UV;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_View;
    vec3 cameraPos;
    float deltaTime;
};

const float PI = 3.14159265359;
uniform samplerCube u_NoiseCubemap;
uniform samplerCube u_NormalCubemap;
uniform vec3 u_TerrainLevels;

vec3 heightToColour(float h) {
    if (h <= u_TerrainLevels.x) {
        return vec3(0.0, 0.0, 1.0);   // water
    } else if (h > u_TerrainLevels.x && h <= u_TerrainLevels.y) {
        return vec3(1.0, 0.9, 0.6);   // sand
    } else if (h > u_TerrainLevels.y && h <= u_TerrainLevels.z) {
        return vec3(0.0, 1.0, 0.0);   // grass and rock
    } else {
        return vec3(1.0, 1.0, 1.0);   // snow
    }
}

void main() {
    vec3 sphericalNormal = normalize(FragPos);

    // TBN matrix is already applied to normal cubemap
    vec3 normal = texture(u_NormalCubemap, sphericalNormal).rgb;
    normal = normalize(normal);

    // Sample height for texturing
    float height = texture(u_NoiseCubemap, sphericalNormal).r;

    vec3 terrainColour = heightToColour(height);

    gPosition = FragPos;
	gNormal = normal;
	gAlbedo = vec4(0.0, texture(u_NoiseCubemap, sphericalNormal).g, 0.0, 1.0);
}