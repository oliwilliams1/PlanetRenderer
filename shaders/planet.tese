#version 410 core

layout (quads, fractional_even_spacing, ccw) in;

out vec3 Normal;
out vec3 FragPos;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    vec3 cameraPos;
    float deltaTime;
};

uniform mat4 m_Model;
uniform float planetScale;

void main() {
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec4 pos0 = gl_in[0].gl_Position;
    vec4 pos1 = gl_in[1].gl_Position;
    vec4 pos2 = gl_in[2].gl_Position;
    vec4 pos3 = gl_in[3].gl_Position;

    vec4 pos = mix(mix(pos0, pos1, u), mix(pos3, pos2, u), v);
    vec3 normalizedPos = normalize(pos.xyz);

    vec4 mPos = m_Model * vec4(normalizedPos * planetScale, 1.0);

    gl_Position = m_ViewProj * mPos;
    Normal = normalizedPos;
    FragPos = mPos.xyz;
}