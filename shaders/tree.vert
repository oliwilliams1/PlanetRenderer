#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in mat4 m_Model;

out vec3 FragPos;
out vec2 UV;
out flat int imposterRow;
out flat mat3 TBN;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_View;
    vec3 cameraPos;
    float deltaTime;
};

uniform float u_TreeScale;

void main() {
    // Calculate the position of the center of the tree for constant rotation matricies for billboarding
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

    // Caluculate the imposter row via dot product (see how simmilar tree up is to cam dir)
    float treeDotProduct = dot(normalize(worldPos.xyz), toCamera);
    vec2 uv = (position.xy + 1.0) / 2.0;

    // Values are not precise, but they work
    if (treeDotProduct < 1.0 && treeDotProduct > 0.96) {
        imposterRow = 0;
    } else if (treeDotProduct < 0.96 && treeDotProduct > 0.76) {
        imposterRow = 1;
    } else if (treeDotProduct < 0.76 && treeDotProduct > 0.5) {
        imposterRow = 2;
    } else if (treeDotProduct < 0.5  && treeDotProduct > 0.24) {
        imposterRow = 3;
    } else {
        imposterRow = 4;
    }

    gl_Position = m_ViewProj * worldPos;
    FragPos = worldPos.xyz;
    UV = uv;
}