#version 330 core

layout(location = 0) in vec3 position;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
};

void main() {
    gl_Position = m_ViewProj * vec4(position, 1.0);
}