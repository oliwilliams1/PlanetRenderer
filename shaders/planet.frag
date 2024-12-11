#version 410 core

in vec3 Normal;
in vec3 FragPos;
in vec2 UV;

out vec4 colour;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    mat4 m_View;
    vec3 cameraPos;
    float deltaTime;
};

const float PI = 3.14159;
uniform sampler2D u_NoiseTexture;
uniform sampler2D u_NormalTexture;
uniform float u_PlanetScale;

vec3 heightToColour(float h) {
    if (h <= 0.0) {
        return vec3(0.0, 0.0, 1.0);   // water
    } else if (h > 0.0 && h <= 0.1) {
        return vec3(1.0, 0.9, 0.6);   // sand
    } else if (h > 0.1 && h <= 0.4) {
        return vec3(0.0, 1.0, 0.0);   // grass
    } else if (h > 0.4 && h <= 0.5) {
        return vec3(0.5, 0.5, 0.6);   // rock
    } else {
        return vec3(1.0, 1.0, 1.0);   // snow
    }
}

void main() {
    // Normalize interpolated normal
    vec3 normal = normalize(Normal);

    float height;
    vec3 displacedNormal;

    if (abs(FragPos.z - 0.0) < 0.05 * u_PlanetScale) {
        float longitude = atan(normal.z, normal.x) / (2.0 * 3.14159265359) + 0.5;
        float latitude = asin(normal.y) / 3.14159265359 + 0.5;

        height = texture(u_NoiseTexture, vec2(longitude, latitude)).r;
        displacedNormal = texture(u_NormalTexture, vec2(longitude, latitude)).rgb;

    } else {
        height = texture(u_NoiseTexture, UV).r;
        displacedNormal = texture(u_NormalTexture, UV).rgb;
    }
    
    normal = normalize(normal * displacedNormal);

    height = height * 2.0 - 1.0;

    // Calculate the light direction and view direction
    vec3 lightDir = normalize(vec3(1, 1, 1));
    vec3 viewDir = normalize(cameraPos - FragPos);
    
    // Ambient component
    float ambient = 0.1;

    // Diffuse component (Lambert's cosine law)
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular component
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); // Shininess factor is 32

    // Combine components
    float result = ambient + diff + spec;

    colour = vec4(result * heightToColour(height), 1.0);
}