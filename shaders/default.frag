#version 460 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;
layout(location = 3) out int  gObjectID;

in vec3 FragPos;
in vec3 Normal;
in vec2 UV;

uniform int u_SSS;
uniform sampler2D u_Albedo;

void main() {
    gPosition = FragPos;

    vec3 normal = normalize(Normal);

    vec4 albedo = texture(u_Albedo, UV);

    if (albedo.a < 0.5) discard;

    gNormal = normal;
    gAlbedo = albedo;

    if (u_SSS == 1) {
        gObjectID = 1;
    } else {
		gObjectID = 0;
	}
}