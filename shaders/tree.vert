#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

out vec3 FragPos;
out vec3 Normal;
out vec2 UV;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    vec3 cameraPos;
    float deltaTime;
    vec3 cameraForward;
    float time;
};

layout(std430, binding = 0) buffer InstanceData {
	mat4[] m_ModelInstance;
};

layout(std430, binding = 1) buffer InstanceIndexData {
	uint[] instanceIndex;
};

uniform mat4 m_Model;

void main() {
    mat4 modelMatix = m_ModelInstance[instanceIndex[gl_InstanceID]] * m_Model;
    vec4 worldPos = modelMatix * vec4(position, 1.0);
    mat4 normalMatrix = transpose(inverse(modelMatix));

	gl_Position = m_ViewProj * worldPos;
    FragPos = worldPos.xyz;
    Normal = normalize(vec3(normalMatrix * vec4(normal, 0.0)));
    UV = uv;
}