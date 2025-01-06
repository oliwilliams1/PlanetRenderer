#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in mat4 m_Model;

out vec3 FragPos;
out vec3 Normal;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_CameraRotation;
    vec3 cameraPos;
    float deltaTime;
};

uniform mat4 m_ModelMaster;

void main() {
    vec4 worldPos = m_Model * m_ModelMaster * vec4(position, 1.0);

    mat3 normalMatrix = transpose(inverse(mat3(m_Model * m_ModelMaster)));

	gl_Position = m_ViewProj * worldPos;
    FragPos = worldPos.xyz;
    Normal = normalize(normalMatrix * normal);
}