#version 460 core
/**
 * @file   menuVertex.vert
 * @brief  
 * 
 * @author 
 * @date   February 2023
 */

uniform mat3 textureMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;

out Vertex {
	vec2 texCoord;
} OUT;

void main() {
	gl_Position = vec4(textureMatrix * position, 1.0);

	OUT.texCoord = texCoord;
}
