#version 430 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout(location = 3) out int  gObjectID;

in vec3 FragPos;
in vec3 Normal;
in vec2 UV;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_CameraRotation;
    vec3 cameraPos;
    float deltaTime;
};

uniform int u_SSS;
uniform sampler2D u_Albedo;

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

bool shouldDiscardPixel(ivec2 pixelCoords, float v) {
    int index = (pixelCoords.x % 8) + (pixelCoords.y % 8) * 8;
    return v < float(dither[index]) / 63.0;
}

void alphaDitherBasedOnDistance() {
    float d = distance(cameraPos, FragPos);
    float v = 1.0;
    float maxDist = 75.0;
    float minDist = 60.0;

    if (d <= maxDist) {
        float t = (d - minDist) / (maxDist - minDist);
        v = smoothstep(0.0, 1.0, t);
        v -= 0.05;
    }

    ivec2 pixelCoords = ivec2(gl_FragCoord.xy);
    if (!shouldDiscardPixel(pixelCoords, v)) discard;
}


void main() {
    alphaDitherBasedOnDistance();
    gPosition = FragPos;

    vec3 normal = normalize(Normal);

    vec4 albedo = texture(u_Albedo, UV);

    const float minDist = 10.0;
    const float maxDist = 40.0;

    float dist = length(FragPos - cameraPos);

    // Dist between [minDist, maxDist] (1.0 to 0.0)
    dist = clamp((maxDist - dist) / (maxDist - minDist), 0.0, 1.0);

    if (albedo.a < dist) discard;

    gNormal = normal;
    gAlbedo = albedo;

    if (u_SSS == 1) {
        gObjectID = 1;
    } else {
		gObjectID = 0;
	}
}