#version 430 core

layout(location = 0) in vec3 position;

out vec3 FragPos;
out vec2 UV;
out flat mat3 TBN;

out flat float LowerRow;
out flat int Col;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_CameraRotation;
    vec3 cameraPos;
    float deltaTime;
};

layout(std430, binding = 0) buffer InstanceData {
	mat4[] m_ModelInstance;
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
    mat4 m_ModelInst = m_ModelInstance[gl_InstanceID];
    vec3 centerWorldPos = m_ModelInst[3].xyz;

    vec3 toCamera = normalize(cameraPos - centerWorldPos);
    vec3 up = m_ModelInst[1].xyz;
    vec3 right = normalize(cross(up, toCamera));
    up = normalize(cross(toCamera, right));

    TBN = mat3(right, up, toCamera);

    vec4 worldPos = mat4(TBN) * vec4(position * u_TreeScale, 1.0) + vec4(centerWorldPos, 0.0);
    
    vec3 normal = normalize(worldPos.xyz);
    float phi = acos(dot(normal, toCamera)) / PI;
    LowerRow = phi * (8.0 - 1.0);

    Col = pseudoRandom(gl_InstanceID);

    gl_Position = m_ViewProj * worldPos;
    FragPos = worldPos.xyz;

    vec2 uv = (position.xy + 1.0) / 2.0;
    UV = uv;
}