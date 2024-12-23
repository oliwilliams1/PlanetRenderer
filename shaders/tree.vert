#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in mat4 m_Model;

out vec3 FragPos;
out vec2 UV;
out flat int imposterRow;

layout(std140) uniform CameraData {
	mat4 m_ViewProj;
	mat4 m_View;
	vec3 cameraPos;
	float deltaTime;
};

uniform float u_TreeScale;

void main() {
	vec4 worldPos = m_Model * vec4(position * u_TreeScale, 1.0);

	vec3 toCamera = normalize(cameraPos - worldPos.xyz);
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, toCamera));
	up = normalize(cross(toCamera, right));

	mat4 rotation = mat4(
		vec4(right, 0.0),
		vec4(up, 0.0),
		vec4(toCamera, 0.0),
		vec4(0.0, 0.0, 0.0, 1.0)
	);

	worldPos = m_Model * rotation * vec4(position * u_TreeScale, 1.0);

	float treeUp = dot(normalize(worldPos.xyz), toCamera);
	vec2 uv = (position.xy + 1.0) / 2.0;
	
	if (treeUp < 1.0 && treeUp > 0.96) {
		imposterRow = 0;
	} else if (treeUp < 0.96 && treeUp > 0.76) {
		imposterRow = 1;
	} else if (treeUp < 0.76 && treeUp > 0.5) {
		imposterRow = 2;
	} else if (treeUp < 0.5 && treeUp > 0.24) {
		imposterRow = 3;
	} else if (treeUp < 0.24 && treeUp > 0.0) {
		imposterRow = 4;
	}

	gl_Position = m_ViewProj * worldPos;
	FragPos = worldPos.xyz;
	UV = uv;
}