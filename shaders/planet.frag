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
uniform sampler2D u_NormalTexture;
uniform float u_PlanetScale;
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
    colour = vec4(texture(u_NoiseCubemap, normalize(Normal)).rgb, 1.0);
}