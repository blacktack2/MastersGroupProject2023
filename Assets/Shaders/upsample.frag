#version 460 core
/**
 * @file   upsample.frag
 * @brief  Fragment shader for upsampling a texture
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform sampler2D sourceTex;

uniform float filterRadius = 5.0;
 
in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main() {
	vec2 filterCoord = vec2(filterRadius);
	fragColour.w = 1.0;
	fragColour.rgb = texture(sourceTex, IN.texCoord).rgb * 4.0;
	fragColour.rgb += (
		texture(sourceTex, IN.texCoord + filterCoord * vec2( 0,  1)).rgb +
		texture(sourceTex, IN.texCoord + filterCoord * vec2(-1,  0)).rgb +
		texture(sourceTex, IN.texCoord + filterCoord * vec2( 1,  0)).rgb +
		texture(sourceTex, IN.texCoord + filterCoord * vec2( 0, -1)).rgb
	) * 2.0;
	fragColour.rgb += (
		texture(sourceTex, IN.texCoord + filterCoord * vec2(-1,  1)).rgb +
		texture(sourceTex, IN.texCoord + filterCoord * vec2( 1,  1)).rgb +
		texture(sourceTex, IN.texCoord + filterCoord * vec2(-1, -1)).rgb +
		texture(sourceTex, IN.texCoord + filterCoord * vec2( 1, -1)).rgb
	);
	fragColour.rgb *= (1.0 / 16.0);
 }
