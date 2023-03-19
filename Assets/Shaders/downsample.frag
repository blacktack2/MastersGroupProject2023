#version 460 core
/**
 * @file   downsample.frag
 * @brief  Fragment shader for downsampling a texture
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform sampler2D sourceTex;

uniform vec2 sourcePixelSize;
 
in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main() {
	fragColour.w = 1.0;
	fragColour.rgb = texture(sourceTex, IN.texCoord).rgb * 0.125;
	fragColour.rgb += (
		texture(sourceTex, IN.texCoord + sourcePixelSize * vec2(-2,  2)).rgb +
		texture(sourceTex, IN.texCoord + sourcePixelSize * vec2( 2,  2)).rgb +
		texture(sourceTex, IN.texCoord + sourcePixelSize * vec2(-2, -2)).rgb +
		texture(sourceTex, IN.texCoord + sourcePixelSize * vec2( 2, -2)).rgb
	) * 0.03125;
	fragColour.rgb += (
		texture(sourceTex, IN.texCoord + sourcePixelSize * vec2( 0,  2)).rgb +
		texture(sourceTex, IN.texCoord + sourcePixelSize * vec2(-2,  0)).rgb +
		texture(sourceTex, IN.texCoord + sourcePixelSize * vec2( 2,  0)).rgb +
		texture(sourceTex, IN.texCoord + sourcePixelSize * vec2( 0, -2)).rgb
	) * 0.0625;
	fragColour.rgb += (
		texture(sourceTex, IN.texCoord + sourcePixelSize * vec2(-1,  1)).rgb +
		texture(sourceTex, IN.texCoord + sourcePixelSize * vec2( 1,  1)).rgb +
		texture(sourceTex, IN.texCoord + sourcePixelSize * vec2(-1, -1)).rgb +
		texture(sourceTex, IN.texCoord + sourcePixelSize * vec2( 1, -1)).rgb
	) * 0.125;
	// Clamp to above 0 to prevent possible 'black box' artefacts
	fragColour.rgb = max(fragColour.rgb, 0.0001);
 }
