#version 460 core
/**
 * @file   modelDisplace.tesc
 * @brief  Tesselation Control shader for rendering model data to the GBuffer
 * with tesselation-based displacement mapping.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

layout(vertices=3) out;

uniform float maxTessLevel = 64;

in Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
} IN[];

out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
} OUT[];

void main() {
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	OUT[gl_InvocationID].colour   = IN[gl_InvocationID].colour;
	OUT[gl_InvocationID].texCoord = IN[gl_InvocationID].texCoord;
	OUT[gl_InvocationID].normal   = IN[gl_InvocationID].normal;
	OUT[gl_InvocationID].tangent  = IN[gl_InvocationID].tangent;
	OUT[gl_InvocationID].binormal = IN[gl_InvocationID].binormal;

	if (gl_InvocationID == 0) {
		gl_TessLevelOuter[0] = maxTessLevel;
		gl_TessLevelOuter[1] = maxTessLevel;
		gl_TessLevelOuter[2] = maxTessLevel;

		gl_TessLevelInner[0] = maxTessLevel;
	}
}
