#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
};

out vec3 FragPos;
out vec3 Normal;

void main() {
    Normal = normal;
    gl_Position = m_ViewProj * vec4(position, 1.0);
}