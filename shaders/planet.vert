#version 410 core

layout(location = 0) in vec3 position;

uniform float u_PlanetScale;

void main() {
    gl_Position = vec4(normalize(position) * u_PlanetScale, 1.0);
}