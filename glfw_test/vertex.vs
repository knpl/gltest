#version 330 core
layout(location = 0) in vec3 vertex_pos;

void main() {
	gl_Position.xyz = vertex_pos;
	gl_Position.w = 1.0;
}