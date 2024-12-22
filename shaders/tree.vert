#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in mat4 m_Model;

out vec3 FragPos;
out flat mat3 TBN;
out vec2 UV;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_View;
    vec3 cameraPos;
    float deltaTime;
};

void main() {
    vec4 worldPos = m_Model * vec4(position * 5.0, 1.0);

    vec3 normal = normalize(worldPos.xyz);

    float theta = atan(normal.y, normal.x);
    vec3 tangent = normalize(vec3(-sin(theta), 0.0, cos(theta)));
    vec3 bitangent = normalize(cross(normal, tangent));
    TBN = mat3(tangent, bitangent, normal);

    gl_Position = m_ViewProj * worldPos;
    FragPos = worldPos.xyz;
    UV = (position.xy + 1.0) / 2.0;
}