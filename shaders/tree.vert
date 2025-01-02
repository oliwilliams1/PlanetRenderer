#version 430 core

layout(location = 0) in vec3 position;

out vec3 FragPos;
out vec2 UV;
out flat mat3 TBN;

out flat float LowerRow;
out flat uint Col;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_CameraRotation;
    vec3 cameraPos;
    float deltaTime;
};

struct Tree {
    vec4 pos;
    uint treeRot;
    uint treeType;
    uint padding[2];
};

layout(std140) buffer InstanceData {
	Tree InstancedPos[];
};

uniform float u_TreeScale;
const float PI = 3.141592653589;

int pseudoRandom(int seed) {
    seed = (seed ^ 0x5bd1e995) + (seed << 15);
    seed = (seed ^ (seed >> 12));
    seed = (seed + (seed << 2)) + (seed << 4);
    seed = (seed ^ (seed >> 17));
    seed = seed * 0x5bd1e995;
    return abs(seed % 8);
}

void main() {
    vec3 instancedPos = InstancedPos[gl_InstanceID].pos.xyz;
    vec3 toCamera = normalize(cameraPos - instancedPos);
    vec3 up = normalize(instancedPos);
    vec3 right = normalize(cross(up, toCamera));
    up = normalize(cross(toCamera, right));

    TBN = mat3(right, up, toCamera);

    vec4 worldPos = m_CameraRotation * vec4(position * u_TreeScale, 1.0);
    worldPos += vec4(instancedPos, 0.0);
    vec3 normal = normalize(worldPos.xyz);
    float phi = acos(dot(normal, toCamera)) / PI;
    LowerRow = phi * (8.0 - 1.0);

    Col = InstancedPos[gl_InstanceID].treeRot;

    gl_Position = m_ViewProj * worldPos;
    FragPos = worldPos.xyz;

    vec2 uv = (position.xy + 1.0) / 2.0;
    UV = uv;
}