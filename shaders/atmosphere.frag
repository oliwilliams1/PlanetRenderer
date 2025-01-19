#version 460 core

layout(location = 0) out vec4 fragColour;

in vec2 UV;

uniform sampler2D u_FragPos;

const vec3 planetCenter = vec3(0.0);
const float planetRadius = 1000.0;
const float planetUpperRadius = 1200.0;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    vec3 cameraPos;
    float deltaTime;
    vec3 cameraForward;
    float time;
};

float sphereSDF(vec3 p, float radius) {
    return length(p) - radius;
}

void main() {
    vec2 uv = UV;

    vec3 fragPos = texture(u_FragPos, uv).xyz;

    float distanceToSurface = sphereSDF(fragPos - planetCenter, planetRadius);
    
    float distanceToAtmosphere = sphereSDF(fragPos - planetCenter, planetUpperRadius);

    float atmosphereFactor = smoothstep(0.0, planetUpperRadius - planetRadius, distanceToSurface);
    atmosphereFactor = pow(atmosphereFactor, 0.1);
    
    // Define colors based on distance
    vec4 atmosphereColor = vec4(0.0, 0.5, 1.0, 1.0);
    vec4 surfaceColor = vec4(0.5, 1.0, 0.5, 1.0);

    vec4 color = mix(surfaceColor, atmosphereColor, atmosphereFactor);

    fragColour = color;
}