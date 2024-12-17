#version 430 core

layout(location = 0) in vec3 position;

out vec3 FragPos;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_View;
    vec3 cameraPos;
    float deltaTime;
};

void main() {
    vec3 pos = position;
    gl_Position = m_ViewProj * vec4(pos, 1.0);
    FragPos = pos;
}