#version 460 core

layout(location = 0) out vec4 fragColour;

in vec2 UV;

uniform sampler2D u_FragPos;

uniform vec3  u_PlanetCenter;
uniform int   u_STEPS;
uniform float u_MinAtmsDistance;
uniform float u_MaxAtmsDistance;
uniform float u_AtmsExpFalloff;
uniform float u_FalloffB;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    vec3 cameraPos;
    float deltaTime;
    vec3 cameraForward;
    float time;
};

// Get ray direction from camera
vec3 getRayDirection(mat4 m) {
    // Unsure of why UV's are flipped, but it works
    vec2 ndc = vec2((1.0 - UV.x) * 2.0 - 1.0, (1.0 - UV.y) * 2.0 - 1.0);

    vec4 clipSpacePos = vec4(ndc, -1.0, 1.0);
    mat4 inverseProj = inverse(m);
    
    vec4 cameraSpacePos = inverseProj * clipSpacePos;
    cameraSpacePos /= cameraSpacePos.w;
    
    return normalize(cameraSpacePos.xyz);
}

// Ray-sphere intersection
// https://gist.github.com/wwwtyro/beecc31d65d1004f5a9d?permalink_comment_id=2982787#gistcomment-2982787
vec2 raySphereIntersect(vec3 r0, vec3 rd, vec3 s0, float sr) {
    float a = dot(rd, rd);
    vec3 s0_r0 = r0 - s0;
    float b = 2.0 * dot(rd, s0_r0);
    float c = dot(s0_r0, s0_r0) - (sr * sr);
    float disc = b * b - 4.0 * a * c;

    if (disc < 0.0) {
        return vec2(-1.0, -1.0); // No intersections
    } else {
        // Calc the two intersection points
        float t1 = (-b - sqrt(disc)) / (2.0 * a);
        float t2 = (-b + sqrt(disc)) / (2.0 * a);

        // Only return valid if intersecion point is in the same dir as the ray
        if (t1 >= 0.0 && t2 >= 0.0) {
            return vec2(t1, t2); // Both intersections are valid
        } else if (t1 >= 0.0) {
            return vec2(t1, -1.0); // Only first intersection is valid
        } else if (t2 >= 0.0) {
            return vec2(t2, -1.0); // Only second intersection is valid
        } else {
            return vec2(-1.0, -1.0); // Both intersections are behind the ray origin
        }
    }
}

float sampleDensity(vec3 p) {
    float d = length(p - u_PlanetCenter);

    if (d <= u_MinAtmsDistance) {
        return 1.0;
    } else if (d >= u_MaxAtmsDistance) {
        return 0.0;
    } else {
        float t = (d - u_MinAtmsDistance) / (u_MaxAtmsDistance - u_MinAtmsDistance);
        return exp(-t * u_AtmsExpFalloff);
    }
}

void main() {
    vec3 fragPos = texture(u_FragPos, UV).xyz;
    float fragDepth = length(fragPos - cameraPos);

    mat4 m_StrippedViewProj = m_ViewProj;
    m_StrippedViewProj[3] = vec4(0.0, 0.0, 0.0, 1.0); // Remove pesky translation from view matrix
    vec3 rayDir = getRayDirection(m_StrippedViewProj);
    rayDir *= -1.0; // Flip ray direction

    // Perform ray-sphere intersection
    vec2 tValues = raySphereIntersect(cameraPos, rayDir, u_PlanetCenter, u_MaxAtmsDistance);

    float closeInterstionPoint = min(tValues.x, tValues.y);
    float farInterstionPoint = max(tValues.x, tValues.y);

    // If ray lands on planet, far intersection point is planet surface
    if (fragPos != vec3(0.0)) { 
        farInterstionPoint = fragDepth;
    }

    // Calculate depth of atmosphere
    float atmsDepth = farInterstionPoint - closeInterstionPoint;

    float density = 0.0;
    for (int i = 0; i < u_STEPS; i++) {
		vec3 p = (cameraPos + rayDir * closeInterstionPoint) + rayDir * i / float(u_STEPS) * atmsDepth;
		density += sampleDensity(p);
    }

    density /= float(u_STEPS);

    // Temp for development view
    atmsDepth /= u_MinAtmsDistance * u_FalloffB;

    density *= atmsDepth;

    fragColour = vec4(vec3(density), 1.0);
}