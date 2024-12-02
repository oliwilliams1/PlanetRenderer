#version 410 core

out vec4 colour;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    vec3 cameraPos;
    float deltaTime;
};

void main() {
    colour = vec4(1.0);
}