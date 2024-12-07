#version 410 core

layout(location = 0) in vec3 position;

out vec3 Normal;
out vec3 FragPos;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    vec3 cameraPos;
    float deltaTime;
};

uniform mat4 m_Model;

void main() {
    vec4 pos = m_Model * vec4(position, 1.0);
    gl_Position = m_ViewProj * pos;

    FragPos = pos.xyz;
    Normal = normalize(mat3(transpose(inverse(m_Model))) * vec3(0.0, 1.0, 0.0));
}