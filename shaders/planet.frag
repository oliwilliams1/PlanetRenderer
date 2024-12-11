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

const float PI = 3.14159;
uniform sampler2D u_NoiseTexture;

vec3 heightToColour(float h) {
    if (h <= 0.0) {
        return vec3(0.0, 0.0, 1.0);   // water
    } else if (h > 0.0 && h <= 0.2) {
        return vec3(1.0, 0.9, 0.6);   // sand
    } else if (h > 0.2 && h <= 0.5) {
        return vec3(0.0, 1.0, 0.0);   // grass
    } else if (h > 0.5 && h <= 0.8) {
        return vec3(0.5, 0.5, 0.5);   // rock
    } else {
        return vec3(1.0, 1.0, 1.0);   // snow
    }
}

void main() {
    // Normalize interpolated normal
    vec3 normal = normalize(Normal);

    float height = texture(u_NoiseTexture, UV).r;
    height = height * 2.0 - 1.0;

    colour = vec4(heightToColour(height), 1.0);
}