#version 460 core
/**
 * @file   combine.frag
 * @brief  Fragment shader for combining elements of the GBuffer into a single
 * scene.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform sampler2D skyboxTex;
uniform sampler2D albedoTex;
uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform sampler2D ssaoTex;
uniform sampler2D normalTex;
uniform sampler2D depthTex;

uniform vec3 ambienceColour;

uniform int mode;

vec3 displayDefault(vec2 texCoord) {
	vec4 diffuse = texture(albedoTex, texCoord);
	if (diffuse.w == 0.0)
		return texture(skyboxTex, texCoord).xyz;
	vec4 normal = texture(normalTex, texCoord);
	if (normal.w == 0.0)
		return diffuse.rgb;
	vec3 light = texture(diffuseTex,  texCoord).rgb;
	vec3 spec  = texture(specularTex, texCoord).rgb;
	float ambientOcclusion = texture(ssaoTex, texCoord).r;

	vec3 result = diffuse.rgb * light;
	result += spec;
	result += diffuse.rgb * ambienceColour;
	result *= ambientOcclusion;
	return result;
}

vec3 displayNormals(vec2 texCoord) {
	return texture(normalTex, texCoord).rgb;
}

vec3 displayDepth(vec2 texCoord) {
	return vec3((texture(depthTex, texCoord).r - 0.98) * (1.0 / 0.02));
}

vec3 displayAlbedo(vec2 texCoord) {
	return texture(albedoTex, texCoord).rgb;
}

vec3 displayDiffuse(vec2 texCoord) {
	return texture(diffuseTex, texCoord).rgb;
}

vec3 displaySpecular(vec2 texCoord) {
	return texture(specularTex, texCoord).rgb;
}

vec3 displayAmbientOcclusion(vec2 texCoord) {
	return texture(ssaoTex, texCoord).rrr;
}

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main() {
	if (mode == 1) {
		fragColour.rgb = displayNormals(IN.texCoord);
	} else if (mode == 2) {
		fragColour.rgb = displayDepth(IN.texCoord);
	} else if (mode == 3) {
		fragColour.rgb = displayAlbedo(IN.texCoord);
	} else if (mode == 4) {
		fragColour.rgb = displayDiffuse(IN.texCoord);
	} else if (mode == 5) {
		fragColour.rgb = displaySpecular(IN.texCoord);
	} else if (mode == 6) {
		fragColour.rgb = displayAmbientOcclusion(IN.texCoord);
	} else {
		fragColour.rgb = displayDefault(IN.texCoord);
	}
	fragColour.a = 1.0;
}
