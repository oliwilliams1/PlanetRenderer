#version 430 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedo;

in vec3 FragPos;
in vec2 UV;
in flat mat3 TBN;

in flat float LowerRow;
in flat int Col;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_CameraRotation;
    vec3 cameraPos;
    float deltaTime;
};

uniform sampler2D u_Albedo;
uniform sampler2D u_Normal;

const int gridSize = 8;
const float ditherPattern[64] = float[](
    0.0,   0.125, 0.875, 0.75,  0.5,   0.625, 0.375, 0.25,
    0.5,   0.625, 0.375, 0.25,  0.75,  0.875, 0.125, 0.0,
    0.875, 0.75,  0.0,   0.125, 0.25,  0.375, 0.625, 0.5,
    0.25,  0.375, 0.625, 0.5,   0.0,   0.125, 0.875, 0.75,
    0.5,   0.625, 0.375, 0.25,  0.75,  0.875, 0.125, 0.0,
    0.75,  0.875, 0.125, 0.0,   0.25,  0.375, 0.625, 0.5,
    0.125, 0.0,   0.5,   0.625, 0.875, 0.75,  0.0,   0.125,
    0.0,   0.125, 0.875, 0.75,  0.5,   0.625, 0.375, 0.25
);

vec2 imposterSampler(vec2 uv, int row, int col) {
    row = gridSize - row - 1;
    vec2 imposterSize = vec2(1.0 / gridSize);
    vec2 imposterOffset = vec2(col, row) * imposterSize;
    return imposterOffset + uv * imposterSize;
}

bool shouldDiscardPixel(ivec2 pixelCoords, float v) {
    int index = (pixelCoords.x % gridSize) + (pixelCoords.y % gridSize) * gridSize;
    return v < ditherPattern[index];
}

void alphaDitherBasedOnDistance() {
    float d = distance(cameraPos, FragPos);
    float v = 1.0;
    float maxDist = 200.0;
    float minDist = 150.0;

    if (d < minDist) {
        discard;
    } else if (d <= maxDist) {
        float t = (d - minDist) / (maxDist - minDist);
        v = smoothstep(0.0, 1.0, t);
    }

    ivec2 pixelCoords = ivec2(gl_FragCoord.xy);
    if (shouldDiscardPixel(pixelCoords, v)) discard;
    }

void main() {
    //alphaDitherBasedOnDistance();
    int lowerRow = int(LowerRow);
    int upperRow = min(lowerRow + 1, gridSize - 1);
    float rowBlendFactor = LowerRow - lowerRow;

    vec2 uvLower = imposterSampler(UV, lowerRow, Col);
    vec2 uvUpper = imposterSampler(UV, upperRow, Col);

    vec4 albedoLower = texture(u_Albedo, uvLower);
    vec4 albedoUpper = texture(u_Albedo, uvUpper);
    vec4 finalAlbedo = mix(albedoLower, albedoUpper, rowBlendFactor);

    if (finalAlbedo.a < 0.5) discard;
    
    vec3 normalUpper = texture(u_Normal, uvUpper).rgb * 2.0 - 1.0;
    vec3 normalLower = texture(u_Normal, uvLower).rgb * 2.0 - 1.0;
    vec3 finalNormal = normalize(TBN * normalize(mix(normalLower, normalUpper, rowBlendFactor)));

    gPosition = FragPos;
    gNormal = vec3(finalNormal);
    gAlbedo = vec4(vec3(1.0), finalAlbedo.a);
}