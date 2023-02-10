#version 460 core
/**
 * @file   hdr.frag
 * @brief  Fragment shader for applying a High Dynamic Range post-processing
 * effect.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform sampler2D sceneTex;

uniform float exposure;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main() {
	vec3 scene = texture(sceneTex, IN.texCoord).rgb;

	vec3 mapped = vec3(1.0) - exp(-scene * exposure);

	fragColour = vec4(mapped, 1.0);
}
