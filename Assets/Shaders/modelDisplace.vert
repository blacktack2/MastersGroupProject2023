#version 460 core
/**
 * @file   modelDisplace.vert
 * @brief  Vertex shader for rendering model data to the GBuffer with
 * tesselation-based displacement mapping.
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

	gl_Position = vec4(position, 1.0);
}
