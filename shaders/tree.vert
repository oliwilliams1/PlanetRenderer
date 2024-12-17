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

    vec3 toCamera = normalize(cameraPos - worldPos.xyz);

    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, toCamera));
    vec3 newUp = cross(toCamera, right);

    mat4 rotationMatrix = mat4(
        vec4(right, 0.0),
        vec4(newUp, 0.0),
        vec4(toCamera, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );

    worldPos = m_Model * rotationMatrix * vec4(position * 4.0, 1.0);

    gl_Position = m_ViewProj * worldPos;

    FragPos = worldPos.xyz;
}