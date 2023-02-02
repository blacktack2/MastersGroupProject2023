#version 330 core

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;

out Vertex {
	vec3 viewDir;
} OUT;

void main() {
	gl_Position = vec4(position, 1.0);
	OUT.viewDir = transpose(mat3(viewMatrix)) * (inverse(projMatrix) * gl_Position).xyz;
}