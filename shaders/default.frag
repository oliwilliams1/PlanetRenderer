#version 430 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

in vec3 FragPos;
in vec3 Normal;

uniform int normalizedNormal;

void main() {
    gPosition = FragPos;

    vec3 normal = normalize(Normal);

    if (normalizedNormal == 1) {
        normal = normal * 0.5 + 0.5;
    }

    gNormal = normal;
    gAlbedo = vec4(1.0);
}