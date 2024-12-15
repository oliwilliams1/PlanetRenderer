#version 410 core

in vec3 Normal;
in vec3 FragPos;
in vec2 UV;

out vec4 colour;

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
    vec3 sphericalNormal = normalize(Normal);
    vec3 normal = texture(u_NormalCubemap, sphericalNormal).rgb;
    normal *= 2.0;
    normal += 1.0;
    normal = normalize(normal);

    float height = texture(u_NoiseCubemap, sphericalNormal).r;
    height = height * 2.0 - 1.0; // [0, 1] -> [-1, 1]

    vec3 terrainColour = heightToColour(height);

    terrainColour *= 0.1 + clamp(max(dot(normal, normalize(vec3(1.0))), 0.0), 0.0, 0.8);

    colour = vec4(terrainColour, 1.0);
}