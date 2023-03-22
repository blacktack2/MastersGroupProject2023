#version 460 core
/**
 * @file   shadowAnimated.vert
 * @brief  
 * 
 * @author 
 * @author Harry Brettell
 * @date   February 2023
 */

uniform mat4 modelMatrix;
uniform mat4 projViewMatrix;

layout(location = 0) in vec3 position;
layout(location = 5) in vec4 jointWeights;
layout(location = 6) in ivec4 jointIndices;

uniform mat4 joints[110];

uniform float frameLerp;

void main(void) {
		vec4 skelPos = vec4(0.0);

	for (int i = 0; i < 4; ++i) {
		int   jointIndex  = jointIndices[i];
		float jointWeight = jointWeights[i];

		skelPos += joints[jointIndex] * vec4(position, 1.0) * jointWeight;
	}
	
	vec4 worldPos = modelMatrix * skelPos;
	gl_Position = projViewMatrix * worldPos;
}