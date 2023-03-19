#version 460 core
/**
 * @file   modelDefault.vert
 * @brief  Default vertex shader for rendering model data to the GBuffer.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec4 tangent;

out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} OUT;

void main() {
	OUT.colour = colour;
	OUT.texCoord = texCoord;

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	vec3 worldNormal  = normalize(normalMatrix * normalize(normal));
	vec3 worldTangent = normalize(normalMatrix * normalize(tangent.xyz));

	OUT.normal   = worldNormal;
	OUT.tangent  = worldTangent;
	OUT.binormal = cross(worldTangent, worldNormal) * tangent.w;

	vec4 worldPos = modelMatrix * vec4(position, 1.0);

	OUT.worldPos = worldPos.xyz;

	gl_Position = projMatrix * viewMatrix * worldPos;
}
