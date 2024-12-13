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

const float PI = 3.14159265359;
uniform sampler2D u_NoiseTexture;
uniform sampler2D u_NormalTexture;
uniform float u_PlanetScale;
uniform vec3 u_TerrainLevels;

vec3 heightToColour(float h) {
    if (h <= u_TerrainLevels.x) {
        return vec3(0.0, 0.0, 1.0);   // water
    } else if (h > u_TerrainLevels.x && h <= u_TerrainLevels.y) {
        return vec3(1.0, 0.9, 0.6);   // sand
    } else if (h > u_TerrainLevels.y && h <= u_TerrainLevels.z) {
        return vec3(0.0, 1.0, 0.0);   // grass and rock
    } else {
        return vec3(1.0, 1.0, 1.0);   // snow
    }
}

void main() {
    vec3 normal = normalize(Normal);

    // Spherical coordinates
    float theta = atan(normal.z, normal.x);
    float phi = asin(normal.y);

    // Calculate tangent and bitangent vectors
    vec3 tangent = normalize(vec3(-sin(theta), 0.0, cos(theta)));
    vec3 bitangent = normalize(cross(normal, tangent));

    // Construct TBN matrix for displacing normal
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    // Caluclate uv coords based on sphercial coords to sample textures
    vec2 uv = vec2(theta / (2.0 * PI) + 0.5, phi / (PI) + 0.5);

    // Sample height in range of [-1, 1]
    float height = texture(u_NoiseTexture, uv).r;
    height = height * 2.0 - 1.0;

    // If not water-level
    // Sample normal from noise [-1, 1]
    vec3 displacedNormal = texture(u_NormalTexture, uv).rgb;
    displacedNormal = normalize(displacedNormal * 2.0 - 1.0);
    // Displace normal properly with correct TBN matrix
    normal = normalize(TBN * displacedNormal);

    // Convert terrain height to colour
    vec3 terrainColour = heightToColour(height);
    
    // Phong shadiing model, calculate light dir and view dir
    vec3 lightDir = normalize(vec3(1, 1, 1));
    vec3 viewDir = normalize(cameraPos - FragPos);

	float ambient = 0.1;

    // Find how simmilar the light is to the normal
    float diff = max(dot(normal, lightDir), 0.0);
    diff = clamp(diff, 0.0, 0.8);

    // Find total contrib
    float shadingContrib = ambient + diff;

    // Apply phong contrib to terrain colour
    colour = vec4(terrainColour * shadingContrib, 1.0);
}