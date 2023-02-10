#version 460 core

uniform mat4 modelMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv

out vertex {
	vec3 worldPos;
	vec2 uv;
}OUT;

void main() {
	OUT.worldPos = (modelMatrix * vec4(position, 1.0)).xyz;
	OUT.uv = uv;
	gl_Position = vec4((uv.xy * 2 - 1), 0, 1);
}