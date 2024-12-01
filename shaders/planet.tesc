#version 410 core

layout (vertices = 4) out;

layout(std140) uniform CameraData {
    mat4 m_ViewProj;
    vec3 cameraPos;
    float deltaTime;
};

uniform float planetScale;
uniform mat4 m_Model;

float cubicInterpolate(float p0, float p1, float p2, float p3, float t) {
    return 0.5 * ((p1) + (p2)) + 
           0.5 * t * ((p2) - (p0) + 
           t * ((2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3) + 
           t * (3.0 * (p1 - p2) + p3 - p0)));
}

void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    if(gl_InvocationID == 0) {
        const int MIN_TESS_LEVEL = 4;
        const int MAX_TESS_LEVEL = 96;
        float MIN_DISTANCE = planetScale * 0.75;
        float MAX_DISTANCE = planetScale * 2.0; // Ensure a face never is smaller than 2x2 pixels
        const float DISTANCE_THRESHOLD = 4000.0;

        // Transform vertices to eye space
        vec4 eyeSpacePos00 = m_ViewProj * m_Model * vec4(normalize(gl_in[0].gl_Position.xyz) * planetScale, 1.0);
        vec4 eyeSpacePos01 = m_ViewProj * m_Model * vec4(normalize(gl_in[1].gl_Position.xyz) * planetScale, 1.0);
        vec4 eyeSpacePos10 = m_ViewProj * m_Model * vec4(normalize(gl_in[2].gl_Position.xyz) * planetScale, 1.0);
        vec4 eyeSpacePos11 = m_ViewProj * m_Model * vec4(normalize(gl_in[3].gl_Position.xyz) * planetScale, 1.0);

        // Calculate distances
        float distance00 = clamp((abs(eyeSpacePos00.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
        float distance01 = clamp((abs(eyeSpacePos01.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
        float distance10 = clamp((abs(eyeSpacePos10.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
        float distance11 = clamp((abs(eyeSpacePos11.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);

        distance00 = pow(distance00, 0.15);
        distance01 = pow(distance01, 0.15);
        distance10 = pow(distance10, 0.15);
        distance11 = pow(distance11, 0.15);

        // Calculate the average distance to the patch
        float averageDistance = (eyeSpacePos00.z + eyeSpacePos01.z + eyeSpacePos10.z + eyeSpacePos11.z) / 4.0;

        // Check if the average distance exceeds the threshold
        if (abs(averageDistance) > DISTANCE_THRESHOLD) {
            gl_TessLevelOuter[0] = 1;
            gl_TessLevelOuter[1] = 1;
            gl_TessLevelOuter[2] = 1;
            gl_TessLevelOuter[3] = 1;
            gl_TessLevelInner[0] = 1;
            gl_TessLevelInner[1] = 1;
            return;
        }

        // Interpolate tessellation levels based on distances
        float tessLevel0 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance10, distance00));
        float tessLevel1 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance00, distance01));
        float tessLevel2 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance01, distance11));
        float tessLevel3 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance11, distance10));

        gl_TessLevelOuter[0] = tessLevel0;
        gl_TessLevelOuter[1] = tessLevel1;
        gl_TessLevelOuter[2] = tessLevel2;
        gl_TessLevelOuter[3] = tessLevel3;

        gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
        gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);
    }
}