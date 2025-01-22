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
uniform vec3  u_ColourContribs;
uniform vec3  u_Colour1;
uniform vec3  u_Colour2;
uniform vec3  u_Colour3;

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

// Ray-sphere intersection, finding two points
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

// For ray-sphere intersections where ray origin is inside the sphere
float intersectRaySphere(vec3 r0, vec3 rd, vec3 sphereCenter, float sphereRadius) {
    vec3 oc = r0 - sphereCenter;

    float a = dot(rd, rd);
    float b = 2.0 * dot(oc, rd);
    float c = dot(oc, oc) - sphereRadius * sphereRadius;

    float discriminant = b * b - 4.0 * a * c;

    if (discriminant < 0.0) {
        return -1.0;
    }

    float t = (-b - sqrt(discriminant)) / (2.0 * a);

    if (dot(oc, oc) < sphereRadius * sphereRadius) {
        return sphereRadius - length(oc);
    }

    // Check if the intersection is in front of the ray
    if (t < 0.0) {
        return -1.0; // Intersection behind the ray
    }

    return t;
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

// Get ray direction from camera's view matrix
vec3 getRayDir() {
    mat4 m_StrippedViewProj = m_ViewProj;
    m_StrippedViewProj[3] = vec4(0.0, 0.0, 0.0, 1.0); // Remove pesky translation from view matrix
    vec3 rayDir = getRayDirection(m_StrippedViewProj);
    rayDir *= -1.0; // Flip ray direction

    return rayDir;
}

vec3 atmsScatter(vec3 rayDir, float closeInterstionPoint, float atmsDepth) {
    vec3 sunDir = normalize(vec3(1.0));

    vec3 colourContribs = vec3(0.0);

    for (int i = 0; i < u_STEPS; i++) {
        // Sample point along ray
        vec3 p = (cameraPos + rayDir * closeInterstionPoint) + rayDir * i / float(u_STEPS) * atmsDepth;

        // Find dist through atms from p to sun
        float distPToAtms = intersectRaySphere(p, -sunDir, u_PlanetCenter, u_MaxAtmsDistance);
    
        // Pre-computations
        vec3 toPlanetCenter = normalize(p - u_PlanetCenter);
        float NdotL = dot(toPlanetCenter, u_SunDir);

        for (int j = 0; j < u_STEPS; j++) {
            vec3 p2 = p + sunDir * j * distPToAtms / float(u_STEPS);

            vec2 closestPoint = closestPointOnRayToSphere(p2, sunDir, u_PlanetCenter, u_MinAtmsDistance);
            float closestPointDepth = clamp(closestPoint.x, -50.0, 50.0) + 50.0;
            closestPointDepth /= 100.0; // [0.0, 1.0]

            float density = sampleDensity(p2);

            // Use temporary variables to avoid repeated calculations
            float densityMulNdotL = density * NdotL;
            colourContribs.r += density * closestPointDepth * u_ColourContribs.r;
            colourContribs.g += densityMulNdotL * closestPointDepth * u_ColourContribs.g;
            colourContribs.g += densityMulNdotL * (1.0 - closestPointDepth) * u_ColourContribs.g;
            colourContribs.b += (density * (1.0 - closestPointDepth) * u_ColourContribs.b) * NdotL;
        }
    }

    // Normalise colour contributions
    colourContribs /= float(u_STEPS * u_STEPS);

    // Calculate final colour
    vec3 colour = vec3(0.0);
    mat3 colours = mat3(u_Colour1, u_Colour2, u_Colour3);

    for (int i = 0; i < 3; i++) {
		colour += colourContribs[i] * colours[i];
	}

    // Further normalize
    atmsDepth /= u_MinAtmsDistance * u_FalloffB;
    colour *= atmsDepth;

    // Correct any out of bounds values
    colour = clamp(colour, 0.0, 1.0);

    return colour;
}

void main() {
    vec3 fragPos = texture(u_FragPos, UV).xyz;
    float fragDepth = length(fragPos - cameraPos);

    vec3 rayDir = getRayDir();

    // Perform ray-sphere intersection
    vec2 intersectionDistances = raySphereIntersect(cameraPos, rayDir, u_PlanetCenter, u_MaxAtmsDistance);

    float closeInterstionPoint = min(intersectionDistances.x, intersectionDistances.y);
    float farInterstionPoint = max(intersectionDistances.x,   intersectionDistances.y);

    // If ray lands on planet, far intersection point is planet surface
    if (fragPos != vec3(0.0)) { 
        farInterstionPoint = fragDepth;
    }

    // Calculate depth of atmosphere
    float atmsDepth = farInterstionPoint - closeInterstionPoint;

    // Get atmosphere colour
    vec3 colour = atmsScatter(rayDir, closeInterstionPoint, atmsDepth);

    // Get colour from render target
    vec4 fragColourIn = texture(u_FragColourIn, UV);

    // Apply atmosphere onto render target
    fragColour = fragColourIn + vec4(colour, 1.0);
}