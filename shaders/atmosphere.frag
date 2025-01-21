#version 460 core

layout(location = 0) out vec4 fragColour;

in vec2 UV;

uniform sampler2D u_FragPos;
uniform sampler2D u_FragColourIn;

uniform vec3  u_PlanetCenter;
uniform int   u_STEPS;
uniform float u_MinAtmsDistance;
uniform float u_MaxAtmsDistance;
uniform float u_AtmsExpFalloff;
uniform float u_FalloffB;
uniform vec3  u_SunDir = normalize(vec3(1.0));

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

// Chatgpt
vec2 closestPointOnRayToSphere(vec3 rayOrigin, vec3 rayDirection, vec3 sphereCenter, float sphereRadius) {
    vec3 normalizedDirection = normalize(rayDirection);
    
    // Vector from ray origin to sphere center
    vec3 oc = sphereCenter - rayOrigin;
    
    // Project oc onto the ray direction to find the closest point
    float t = dot(oc, normalizedDirection);
    
    // Calculate the closest point on the ray
    vec3 closestPoint = rayOrigin + t * normalizedDirection;

    // Calculate the distance from the closest point to the sphere center
    float distanceToSphere = length(closestPoint - sphereCenter) - sphereRadius;
    
    return vec2(distanceToSphere, closestPoint);
}

float sampleDensity(vec3 p) {
    float d = length(p - u_PlanetCenter);

    if (d <= u_MinAtmsDistance) {
        return 0.0;
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

    vec3 sunDir = normalize(vec3(1.0));

    float density = 0.0;
    for (int i = 0; i < u_STEPS; i++) {
		vec3 p = (cameraPos + rayDir * closeInterstionPoint) + rayDir * i / float(u_STEPS) * atmsDepth;
        for (int j = 0; j < u_STEPS; j++) {
            vec3 p2 = p + rayDir * j / float(u_STEPS);

            float closestPointDepth = 1.0;
            // If p2 is on the "dark side" of the planet, perform nesscary calculations for rim falloff lighting
            if (dot((p2 - u_PlanetCenter), sunDir) < 0.0) {
                vec2 closestPoint = closestPointOnRayToSphere(p2, sunDir, u_PlanetCenter, u_MinAtmsDistance);
                closestPointDepth = closestPoint.x;
                closestPointDepth = clamp(closestPointDepth, -50.0, 50.0);
                closestPointDepth += 50.0;
                closestPointDepth /= 50.0 * 2.0;
            } 
            
		    density += sampleDensity(p2) * closestPointDepth;
        }
    }

    density /= float(u_STEPS * u_STEPS);

    // Temp for development view
    atmsDepth /= u_MinAtmsDistance * u_FalloffB;

    density *= atmsDepth;

    vec4 colour = vec4(vec3(1.0), density);
    vec4 fragColourIn = texture(u_FragColourIn, UV);

    density = clamp(density, 0.0, 1.0);

    // Alpha blend atmosphere onto scene
    fragColour = fragColourIn + density;
}