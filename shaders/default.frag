#version 430 core

in vec2 UV;

layout (location = 0) out vec4 colour;

void main() {
	colour = vec4(UV.x, 0.0, UV.y, 1.0);
}