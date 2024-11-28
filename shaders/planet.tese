#version 410 core

layout (quads, equal_spacing, ccw) in;

out vec3 Normal;

void main() {
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec4 pos0 = gl_in[0].gl_Position;
    vec4 pos1 = gl_in[1].gl_Position;
    vec4 pos2 = gl_in[2].gl_Position;
    vec4 pos3 = gl_in[3].gl_Position;

    vec4 pos = mix(mix(pos0, pos1, u), mix(pos3, pos2, u), v);

    gl_Position = pos;
}