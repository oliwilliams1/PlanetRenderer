#version 330 core

in vec3 Normal;

out vec4 color;

void main() {
    vec3 viewPos = vec3(0.0, 0.0, 4.0);

    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));

    vec3 ambient = vec3(0.1);
    vec3 diffuse = vec3(max(dot(Normal, lightDir), 0.0));

    vec3 result = ambient + diffuse;
    color = vec4(result * vec3(1.0, 0.0, 0.0), 1.0);
}