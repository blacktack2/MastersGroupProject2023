#version 460 core
/**
 * @file   skybox.vert
 * @brief  Vertex shader for a procedural skybox. See skybox.frag for more
 * info.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

layout(location = 0) in vec3 position;

out Vertex {
	vec3 viewDir;
} OUT;

void main() {
	gl_Position = vec4(position, 1.0);
	OUT.viewDir = transpose(mat3(viewMatrix)) * (inverse(projMatrix) * gl_Position).xyz;
}