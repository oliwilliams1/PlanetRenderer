#version 430 core

layout (location = 0) out vec4 colour;

in vec2 UV;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform isampler2D gObjectID;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_CameraRotation;
    vec3 cameraPos;
    float deltaTime;
};

// https://stackoverflow.com/questions/15095909/from-rgb-to-hsv-in-opengl-glsl
vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    vec3 lightColour = vec3(0.6);

    vec3 position = texture(gPosition, UV).xyz;
    vec3 normal = normalize(texture(gNormal, UV).xyz);
    vec3 albedo = texture(gAlbedo, UV).rgb;

    vec3 ambient = 0.1 * albedo;

    vec3 lightDir = vec3(1.0);
    vec3 viewDir = normalize(cameraPos - position);
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * albedo * lightColour;

    int objectID = texture(gObjectID, UV).r;

    if (objectID == 1) {
        float sss = exp(-3.0 * abs(dot(normal, lightDir)));

        vec3 scatterColour = rgb2hsv(albedo);
        scatterColour.x = mod(scatterColour.x + (-22.0 / 360.0), 1.0);
        scatterColour.y = clamp(scatterColour.y + 0.43, 0.0, 1.0);

        diffuse += sss * hsv2rgb(scatterColour) * lightColour;
    }

    vec3 finalColor = ambient + diffuse;

    colour = vec4(finalColor, 1.0);
}