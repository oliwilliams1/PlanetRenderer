#version 410 core

in vec3 Normal;
in vec3 FragPos;

out vec4 colour;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    vec3 cameraPos;
    float deltaTime;
};

uniform vec3 planetColour;
const float PI = 3.14159;

void main() {
    // Normalize interpolated normal
    vec3 norm = normalize(Normal);

    // Calculate spherical coords from normal
    float latitude = asin(norm.y) * (180.0 / PI);
    float longitude = atan(norm.z, norm.x) * (180.0 / PI);

    float U = (longitude + 180.0) / 360.0;
    float V = (90.0 - latitude) / 180.0;

    // Phong lighting
    vec3 lightDirection = normalize(vec3(1.0, 1.0, 1.0));
    vec3 viewDir = normalize(cameraPos - FragPos);
    
    vec3 ambient = vec3(0.1);

    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = vec3(diff);

    vec3 halfDir = normalize(lightDirection + viewDir);
    float spec = pow(max(dot(norm, halfDir), 0.0), 32.0);
    vec3 specular = vec3(1.0) * spec;

    vec3 result = ambient + diffuse + specular;

    colour = vec4(vec3(1.0) * result, 1.0);
}