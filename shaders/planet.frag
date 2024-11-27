#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 colour;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    vec3 cameraPos;
    float padding;
};

uniform vec3 planetColour;

void main() {
    vec3 norm = normalize(Normal);

    vec3 lightDirection = normalize(vec3(1.0, 1.0, 1.0));
    vec3 viewDir = normalize(cameraPos - FragPos);
    
    vec3 ambient = vec3(0.1) * planetColour;

    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * planetColour;

    vec3 halfDir = normalize(lightDirection + viewDir);
    float spec = pow(max(dot(norm, halfDir), 0.0), 32.0);
    vec3 specular = vec3(1.0) * spec;

    vec3 result = ambient + diffuse + specular;
    
    colour = vec4(result, 1.0);
}