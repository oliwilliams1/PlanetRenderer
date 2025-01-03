#version 430 core

layout (location = 0) out vec4 colour;

in vec2 UV;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_CameraRotation;
    vec3 cameraPos;
    float deltaTime;
};

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

    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), 32.0);
    vec3 specular = lightColour * spec;

    vec3 finalColor = ambient + diffuse;
    colour = vec4(finalColor, 1.0);
}