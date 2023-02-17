#version 460 core
/**
 * @file   ssaoBlur.vert
 * @brief  Vertex shader for applying ssao-optimized blur to a texture.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
 
layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoord;

out Vertex {
	vec2 texCoord;
} OUT;

void main() {
	gl_Position = vec4(position, 1.0);
	OUT.texCoord = texCoord;
}
