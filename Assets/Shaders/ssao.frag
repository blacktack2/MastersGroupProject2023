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
uniform float bias = 0.0;

in Vertex {
	vec2 texCoord;
} IN;

out float fragColour;

void main() {
	mat4 inverseProj = inverse(projMatrix);

	float depth = texture(depthTex, IN.texCoord).r;
	vec3 ndcPos = vec3(IN.texCoord, depth) * 2.0 - 1.0;
	vec4 viewUnscaled = inverseProj * vec4(ndcPos, 1.0);
	vec3 viewPos      = viewUnscaled.xyz / viewUnscaled.w;

	vec3 random = normalize(texture(noiseTex, IN.texCoord * noiseScale).xyz) * 2.0 - 1.0;

	vec3 worldNormal = normalize(texture(normalTex, IN.texCoord).xyz) * 2.0 - 1.0;
	vec3 viewNormal = mat3(viewMatrix) * worldNormal;
	viewNormal = normalize(viewNormal);

	vec3 tangent  = normalize(random - viewNormal * dot(random, viewNormal));
	vec3 binormal = cross(tangent, viewNormal);

	mat3 TBN = mat3(tangent, binormal, viewNormal);

	float occlusion = 0.0;
	for (uint i = 0; i < kernels.length(); i++) {
		vec3 samplePos = TBN * kernels[i];
		samplePos = viewPos + samplePos * radius;

		vec4 offset = projMatrix * vec4(samplePos, 1.0);
		offset.xyz /= offset.w;
		offset.xy = offset.xy * 0.5 + 0.5;

		float sampleDepth = texture(depthTex, offset.xy).r;
		vec4 depthPos = inverseProj * vec4(samplePos.xy, sampleDepth * 2.0 - 1.0, 1.0);
		depthPos.xyz /= depthPos.w;

		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(viewPos.z - depthPos.z));
		occlusion += ((depthPos.z >= samplePos.z + bias) ? 1.0 : 0.0) * rangeCheck;
	}
	fragColour = 1.0 - (occlusion / kernels.length());
}
