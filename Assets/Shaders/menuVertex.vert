#version 460 core

uniform mat4 modelMatrix ;
uniform mat4 viewMatrix ;
uniform mat4 projMatrix ;
uniform mat4 textureMatrix ;

layout(location = 0) in vec3 position ;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord ;

out Vertex {
	vec2 texCoord ;
	vec4 colour;
	vec3 normal;
	vec3 tangent;
	vec3 worldPos;
} OUT;

void main (void) {
	mat4 mvp = projMatrix * viewMatrix * modelMatrix ;
	gl_Position = mvp * vec4(position, 1.0);

	OUT.texCoord = (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
	OUT.colour = colour;
}
