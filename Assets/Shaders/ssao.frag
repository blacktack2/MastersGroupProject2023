#version 460 core
/**
 * @file   ssao.frag
 * @brief  Fragment shader for generating screen-space ambient occlusion data.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

layout (std430, binding = 1) buffer KernelBuffer {
	vec3 kernels[];
};

uniform sampler2D depthTex;
uniform sampler2D normalTex;
uniform sampler2D noiseTex;

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec2 noiseScale;

uniform float radius = 0.5;
uniform float bias = 0.025;

in Vertex {
	vec2 texCoord;
} IN;

out float fragColour;

void main() {
	mat4 projView = projMatrix * viewMatrix;

	float depth = texture(depthTex, IN.texCoord).r;
	vec3 ndcPos = vec3(IN.texCoord, depth) * 2.0 - 1.0;
	vec4 viewPos = inverse(projMatrix) * vec4(ndcPos, 1.0);
	viewPos.xyz /= viewPos.w;

	vec3 random = normalize(texture(noiseTex, IN.texCoord * noiseScale).xyz);
	vec3 normal = normalize(texture(normalTex, IN.texCoord).xyz);
	vec3 tangent  = normalize(random - normal * dot(random, normal));
	vec3 binormal = cross(tangent, normal);
	mat3 TBN = mat3(tangent, binormal, normal);

	float occlusion = 0.0;
	for (uint i = 0; i < 64; i++) {
		vec3 samplePos = TBN * kernels[i];
		samplePos = viewPos.xyz + samplePos * radius;

		vec4 offset = projMatrix * vec4(samplePos, 1.0);
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;

		float sampleDepth = texture(depthTex, offset.xy).r;
		vec3 sampleNDC = vec3(offset.xy, depth) * 2.0 - 1.0;
		vec4 sampleView = inverse(projMatrix) * vec4(sampleNDC, 1.0);
		sampleView.xyz /= sampleView.w;

		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(viewPos.z - sampleView.z));
		occlusion += ((sampleView.z >= samplePos.z + bias) ? 1.0 : 0.0) * rangeCheck;

		//fragColour = vec4(-sampleView.xyz, 1.0);
	}
	//fragColour = (occlusion / kernels.length());
	fragColour = 1.0 - (occlusion / kernels.length());
}
