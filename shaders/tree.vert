#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in mat4 m_Model;

out vec3 FragPos;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_View;
    vec3 cameraPos;
    float deltaTime;
};

void main() {
    vec4 worldPos = m_Model * vec4(position, 1.0);

    gl_Position = m_ViewProj * worldPos;
    FragPos = worldPos.xyz;
}