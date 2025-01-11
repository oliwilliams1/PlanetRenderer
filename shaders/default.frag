#version 430 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout(location = 3) out int  gObjectID;

in vec3 FragPos;
in vec3 Normal;

uniform int u_SSS;

void main() {
    gPosition = FragPos;

    vec3 normal = normalize(Normal);

    gNormal = normal;
    gAlbedo = vec4(1.0);

    if (u_SSS == 1) {
        gObjectID = 1;
    } else {
		gObjectID = 0;
	}
}