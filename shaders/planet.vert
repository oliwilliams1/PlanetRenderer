#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 Normal;
out vec3 FragPos;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    vec3 cameraPos;
    float deltaTime;
};

uniform mat4 m_Model;

void main() {
    vec4 pos = m_ViewProj * m_Model * vec4(position * 100.0, 1.0);
    gl_Position = pos;
    FragPos = pos.xyz;
    Normal = normalize(position);
}