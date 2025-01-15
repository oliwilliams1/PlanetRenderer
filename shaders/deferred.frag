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

// Basic SSS, not realistic, but works as concept
vec3 SSS(vec3 normal, vec3 lightDir, vec3 albedo, vec3 lightColour) {
    float sss = exp(-3.0 * abs(dot(normal, lightDir)));

    vec3 scatterColour = rgb2hsv(albedo);
    scatterColour.x = mod(scatterColour.x + (-10.0 / 360.0), 1.0);
    scatterColour.y = clamp(scatterColour.y + 0.43, 0.0, 1.0);

    return sss * hsv2rgb(scatterColour) * lightColour;
}

// Add backlit lighting to billboards because trees are volumes, not flat surfaces
vec3 BacklitLightingBillboard(float frontDiff, vec3 normal, vec3 lightDir, vec3 albedo, vec3 lightColour) {
    float backDiff = max(dot(normal, -lightDir), 0.0);
    backDiff = clamp(backDiff * 1.5, 0.0, 1.0);

    float blendFact = 0.8;
    vec3 treeDiffuse = mix(albedo * lightColour * frontDiff, albedo * lightColour * backDiff, blendFact);

    return treeDiffuse;
}

void main() {
    vec3 lightColour = vec3(1.0);

    vec3 position = texture(gPosition, UV).xyz;
    vec3 normal = normalize(texture(gNormal, UV).xyz);
    vec3 albedo = texture(gAlbedo, UV).rgb;
    int objectID = texture(gObjectID, UV).r;

    vec3 lightDir = vec3(1.0);
    lightDir = normalize(lightDir);
    vec3 viewDir = normalize(cameraPos - position);

    vec3 ambient = 0.1 * albedo;
    
    float frontDiff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = frontDiff * albedo * lightColour;

    // Add SSS to foliage, billboard or not
    if (objectID == 1 || objectID == 2) {
        diffuse += SSS(normal, lightDir, albedo, lightColour);
    }

    // Add backlit lighting to tree billboard
    if (objectID == 2) {
        diffuse += BacklitLightingBillboard(frontDiff, normal, lightDir, albedo, lightColour);
    }

    // Specular
	//vec3 reflectDir = reflect(-lightDir, normal);
	//float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), 10.0);
	//vec3 specular = specularStrength * lightColour;

    // Add shadows to the planet
    diffuse *= max(dot(normalize(position), lightDir), 0.0);

    // Combine all contribs
    vec3 finalColor = ambient + diffuse;

    colour = vec4(finalColor, 1.0);
}