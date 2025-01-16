#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

out vec3 FragPos;
out vec3 Normal;
out vec2 UV;

layout(std430, binding = 0) buffer ModelMatrixInstanceData {
    mat4 m_Model[];
};

layout(std430, binding = 1) buffer NormalMatrixInstanceData {
    mat4 m_Normal[];
};

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_CameraRotation;
    vec3 cameraPos;
    float deltaTime;
};

uniform mat4 m_ModelMaster;

void main() {
    mat4 instanceModel = m_Model[gl_InstanceID];

    vec4 worldPos = instanceModel * m_ModelMaster * vec4(position, 1.0);
    
    mat4 instanceNormal = m_Normal[gl_InstanceID];
    vec3 worldNormal = normalize(vec3(instanceNormal * vec4(normal, 0.0)));
    Normal = normalize(worldNormal);

    gl_Position = m_ViewProj * worldPos;

    FragPos = worldPos.xyz;
    UV = uv;
}