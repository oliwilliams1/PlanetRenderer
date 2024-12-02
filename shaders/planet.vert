#version 410 core

layout(location = 0) in vec3 position;

uniform mat4 m_Model;

void main() {    
    gl_Position = m_Model * vec4(position, 1.0);
}