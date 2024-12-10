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
uniform sampler2D u_NoiseTexture;
uniform float u_PlanetScale;

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

    float longitude = atan(normal.z, normal.x) / (2.0 * 3.14159265359) + 0.5;
    float latitude = asin(normal.y) / 3.14159265359 + 0.5;

    float height = texture(u_NoiseTexture, vec2(longitude, latitude)).r;
    height -= 0.5;
    vec3 displacedPosition = (normalize(mPos.xyz) * u_PlanetScale) + height * (u_PlanetScale * 0.33)  * normal;

    gl_Position = m_ViewProj * vec4(displacedPosition, 1.0); // Transform into cubesphere
    Normal = normal;
    FragPos = mPos.xyz;
}