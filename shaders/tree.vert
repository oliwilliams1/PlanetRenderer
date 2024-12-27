#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in mat4 m_Model;

out vec3 FragPos;
out vec2 UV;
out flat int lowerRow;
out flat int upperRow;
out flat float blendFactor;
out flat mat3 TBN;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_View;
    vec3 cameraPos;
    float deltaTime;
};

uniform float u_TreeScale;
const float PI = 3.141592653589;

void main() {
    vec4 centerWorldPos = m_Model * vec4(vec3(0.0), 1.0);
    vec3 toCamera = normalize(cameraPos - centerWorldPos.xyz);
    vec3 up = normalize(centerWorldPos.xyz);
    vec3 right = normalize(cross(up, toCamera));
    up = normalize(cross(toCamera, right));

    mat4 rotation = mat4(
        vec4(right, 0.0),
        vec4(up, 0.0),
        vec4(toCamera, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );

    TBN = mat3(rotation);
    vec4 worldPos = m_Model * rotation * vec4(position * u_TreeScale, 1.0);

    float treeDotProduct = dot(normalize(worldPos.xyz), toCamera);
    treeDotProduct = clamp(treeDotProduct, 0.2, 1.0); // to work with my crazy implementation
    float angle = acos(treeDotProduct);
    angle /= (PI / 2.0);

    lowerRow = int(angle * (8.0 - 1.0));
    upperRow = min(lowerRow + 1, 8 - 1);
    blendFactor = (angle * (8.0 - 1.0)) - float(lowerRow);

    vec2 uv = (position.xy + 1.0) / 2.0;
    gl_Position = m_ViewProj * worldPos;
    FragPos = worldPos.xyz;
    UV = uv;
}