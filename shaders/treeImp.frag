#version 430 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedo;
layout(location = 3) out int  gObjectID;

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
const int dither[64] = int[64](
    0, 32, 8, 40, 2, 34, 10, 42,
    48, 16, 56, 24, 50, 18, 58, 26,
    12, 44, 4, 36, 14, 46, 6, 38,
    60, 28, 52, 20, 62, 30, 54, 22,
    3, 35, 11, 43, 1, 33, 9, 41,
    51, 19, 59, 27, 49, 17, 57, 25,
    15, 47, 7, 39, 13, 45, 5, 37,
    63, 31, 55, 23, 61, 29, 53, 21
);

vec2 imposterSampler(vec2 uv, int row, int col) {
    row = gridSize - row - 1;
    vec2 imposterSize = vec2(1.0 / gridSize);
    vec2 imposterOffset = vec2(col, row) * imposterSize;
    return imposterOffset + uv * imposterSize;
}

bool shouldDiscardPixel(ivec2 pixelCoords, float v) {
    int index = (pixelCoords.x % 8) + (pixelCoords.y % 8) * 8;
    return v < float(dither[index]) / 63.0;
}

void alphaDitherBasedOnDistance() {
    float d = distance(cameraPos, FragPos);
    float v = 1.0;
    float maxDist = 75.0;
    float minDist = 60.0;

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
    alphaDitherBasedOnDistance(); // Dither based on distance to camera so high LOD tree model will be rendered

    int lowerRow = int(LowerRow);
    int upperRow = min(lowerRow + 1, gridSize - 1);
    float rowBlendFactor = LowerRow - lowerRow;

    vec2 uvLower = imposterSampler(UV, lowerRow, Col);
    vec2 uvUpper = imposterSampler(UV, upperRow, Col);

    vec4 albedoLower = texture(u_Albedo, uvLower);
    vec4 albedoUpper = texture(u_Albedo, uvUpper);
    vec4 finalAlbedo = mix(albedoLower, albedoUpper, rowBlendFactor);

    ivec2 pixelCoords = ivec2(gl_FragCoord.xy);
    if (finalAlbedo.a < 0.1) discard; // Remove weird artifacts
    if (shouldDiscardPixel(pixelCoords, finalAlbedo.a)) discard; // Dither for blending between rows
    
    vec3 normalUpper = texture(u_Normal, uvUpper).rgb * 2.0 - 1.0;
    vec3 normalLower = texture(u_Normal, uvLower).rgb * 2.0 - 1.0;
    vec3 finalNormal = normalize(TBN * normalize(mix(normalLower, normalUpper, rowBlendFactor)));

    gPosition = FragPos;
    gNormal = vec3(finalNormal);
    gAlbedo = vec4(finalAlbedo.rgb, 1.0);
    gObjectID = 2;
}