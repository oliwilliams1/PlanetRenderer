#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    vec3 cameraPos;
    float time;
};

uniform mat4 m_Matrix;

out vec3 Normal;
out vec3 FragPos;

void main() {
    vec3 distortedPosition = position + sin(time * 10.0 + position.x * 10.0) * 0.01;
    vec4 modelPosition = m_Matrix * vec4(distortedPosition, 1.0);
    
    gl_Position = m_ViewProj * modelPosition;

    Normal = normalize(mat3(m_Matrix) * normal);
    FragPos = modelPosition.xyz;
}