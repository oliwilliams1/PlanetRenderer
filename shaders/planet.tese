#version 410 core

layout (quads, fractional_even_spacing, ccw) in;

out vec3 Normal;
out vec3 FragPos;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_View;
    vec3 cameraPos;
    float deltaTime;
};

uniform mat4 m_Model;
uniform samplerCube u_NoiseCubemap;
uniform samplerCube u_NormalCubemap;
uniform float u_PlanetScale;
uniform float u_Amplitude;

void main() {
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec4 pos0 = gl_in[0].gl_Position;
    vec4 pos1 = gl_in[1].gl_Position;
    vec4 pos2 = gl_in[2].gl_Position;
    vec4 pos3 = gl_in[3].gl_Position;

    vec4 pos = mix(mix(pos0, pos1, u), mix(pos3, pos2, u), v);
    vec4 mPos = m_Model * pos;

    vec3 normal = normalize(mPos.xyz);

    float height = texture(u_NoiseCubemap, normal).r;
    height += 1.0;

    vec3 displacedPosition = (normalize(mPos.xyz) * u_PlanetScale) + height * (u_PlanetScale * u_Amplitude)  * normal;

    gl_Position = m_ViewProj * vec4(displacedPosition, 1.0); // Transform into cubesphere
    Normal = transpose(inverse(mat3(m_Model))) * normal;
    FragPos = mPos.xyz;
}