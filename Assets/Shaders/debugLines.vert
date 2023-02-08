#version 460 core
/**
 * @file   debugLines.vert
 * @brief  Vertex shader for drawing debug lines.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;

uniform mat4 viewProjMatrix;

out Vertex {
	vec4 colour;
} OUT;

void main() {
	gl_Position = viewProjMatrix * vec4(position, 1.0);
	OUT.colour  = colour;
}