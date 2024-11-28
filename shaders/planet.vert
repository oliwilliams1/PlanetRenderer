#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform float planetScale;

void main() {    
    gl_Position = vec4(position * planetScale, 1.0);
}