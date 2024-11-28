#version 410 core

layout (vertices = 4) out;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    vec3 cameraPos;
    float time;
};

uniform float planetScale;

const int MIN_TES = 2;
const int MAX_TES = 32;
const float MIN_DIST = 0;
float MAX_DIST = planetScale * 10.0;

void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    float dist0 = length(gl_in[0].gl_Position.xyz - cameraPos);
    float dist1 = length(gl_in[1].gl_Position.xyz - cameraPos);
    float dist2 = length(gl_in[2].gl_Position.xyz - cameraPos);
    float dist3 = length(gl_in[3].gl_Position.xyz - cameraPos);

    dist0 = clamp(dist0, MIN_DIST, MAX_DIST);
    dist1 = clamp(dist1, MIN_DIST, MAX_DIST);
    dist2 = clamp(dist2, MIN_DIST, MAX_DIST);
    dist3 = clamp(dist3, MIN_DIST, MAX_DIST);

    float decayFactor = 0.2;
    int tes0 = int(mix(MAX_TES, MIN_TES, pow(dist0 / MAX_DIST, decayFactor)));
    int tes1 = int(mix(MAX_TES, MIN_TES, pow(dist1 / MAX_DIST, decayFactor)));
    int tes2 = int(mix(MAX_TES, MIN_TES, pow(dist2 / MAX_DIST, decayFactor)));
    int tes3 = int(mix(MAX_TES, MIN_TES, pow(dist3 / MAX_DIST, decayFactor)));

    gl_TessLevelOuter[0] = tes0;
    gl_TessLevelOuter[1] = tes1;
    gl_TessLevelOuter[2] = tes2;
    gl_TessLevelOuter[3] = tes3;

    gl_TessLevelInner[0] = max(tes1, tes3);
    gl_TessLevelInner[1] = max(tes0, tes2);
}