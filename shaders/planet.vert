#version 410 core

layout(location = 0) in vec3 position;

uniform float planetScale = 300.0;

void main() {
    gl_Position = vec4(normalize(position) * planetScale, 1.0);
}