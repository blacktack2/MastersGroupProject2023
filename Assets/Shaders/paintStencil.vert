#version 460 core

uniform mat4 modelMatrix;

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 uv;

out vertex {
	vec3 worldPos;
} OUT;

void main() {
	vec4 worldPos = (modelMatrix * vec4(position, 1.0));
	OUT.worldPos = worldPos.xyz;
	gl_Position = vec4((uv.xy * 2 - 1), 0, 1);
}