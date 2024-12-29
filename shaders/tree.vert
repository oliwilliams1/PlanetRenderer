#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in mat4 m_Model;

out vec3 FragPos;
out vec2 UV;
out flat mat3 TBN;

out flat int LowerRow;
out flat int UpperRow;
out flat float RowBlendFactor;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_CameraRotation;
    vec3 cameraPos;
    float deltaTime;
};

uniform float u_TreeScale;
const float PI = 3.141592653589;

void main() {
    vec4 centerWorldPos = m_Model * vec4(vec3(0.0), 1.0);
    vec3 toCamera = normalize(cameraPos - centerWorldPos.xyz);
    vec3 treeUp = normalize(centerWorldPos.xyz);
    vec3 right = normalize(cross(treeUp, toCamera));
    vec3 up = normalize(cross(toCamera, right));

    mat4 rotation = mat4(
        vec4(right, 0.0),
        vec4(up, 0.0),
        vec4(toCamera, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );

    TBN = mat3(rotation);

    vec4 worldPos = m_Model * m_CameraRotation * vec4(position * u_TreeScale, 1.0);

    float phi = acos(dot(normalize(worldPos.xyz), toCamera)) / PI;

    LowerRow = int(phi * (8.0 - 1.0));
    UpperRow = min(LowerRow + 1, 8 - 1);
    RowBlendFactor = (phi * (8.0 - 1.0)) - float(LowerRow);

    vec2 uv = (position.xy + 1.0) / 2.0;
    gl_Position = m_ViewProj * worldPos;
    FragPos = worldPos.xyz;
    UV = uv;
}