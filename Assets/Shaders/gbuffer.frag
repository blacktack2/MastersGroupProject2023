#version 460 core
/**
 * @file   gbuffer.frag
 * @brief  Fragment shader for combining elements of the GBuffer into a single
 * image.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform sampler2D skyboxTex;
uniform sampler2D diffuseTex;
uniform sampler2D diffuseLightTex;
uniform sampler2D specularLightTex;
uniform sampler2D normalTex;
uniform sampler2D depthTex;

uniform vec3 ambienceColour;

uniform int mode;

vec3 displayDefault(vec2 texCoord) {
	vec4 diffuse = texture(diffuseTex, texCoord);
	if (diffuse.w == 0.0)
		return texture(skyboxTex, texCoord).xyz;
	vec4 normal = texture(normalTex, texCoord);
	if (normal.w == 0.0)
		return diffuse.rgb;
	vec3 light = texture(diffuseLightTex,  texCoord).rgb;
	vec3 spec  = texture(specularLightTex, texCoord).rgb;

	vec3 result = diffuse.rgb * light;
	result += spec;
	result += diffuse.rgb * ambienceColour;
	return result;
}

vec3 displayNormals(vec2 texCoord) {
	return texture(normalTex, texCoord).rgb;
}

vec3 displayDepth(vec2 texCoord) {
	return vec3((texture(depthTex, texCoord).r - 0.98) * (1.0 / 0.02));
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
	} else {
		fragColour.rgb = displayDefault(IN.texCoord);
	}
	fragColour.a = 1.0;
}
