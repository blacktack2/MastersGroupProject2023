#version 460 core
/**
 * @file   modelAnimated.vert
 * @brief  
 * 
 * @author 
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec4 tangent;
layout(location = 5) in vec4 jointWeights;
layout(location = 6) in ivec4 jointIndices;

uniform mat4 joints[110];

uniform float frameLerp;

out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
} OUT;

void main(void) {
	OUT.colour = colour;
	OUT.texCoord = texCoord;

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	vec3 worldNormal  = normalize(normalMatrix * normalize(normal));
	vec3 worldTangent = normalize(normalMatrix * normalize(tangent.xyz));

	OUT.normal   = worldNormal;
	OUT.tangent  = worldTangent;
	OUT.binormal = cross(worldTangent, worldNormal) * tangent.w;

	vec4 skelPos = vec4(0.0);

	for (int i = 0; i < 4; ++i) {
		int   jointIndex  = jointIndices[i];
		float jointWeight = jointWeights[i];

		skelPos += joints[jointIndex] * vec4(position, 1.0) * jointWeight;
	}
	
	vec4 worldPos = modelMatrix * skelPos;
	gl_Position = projMatrix * viewMatrix * worldPos;
}