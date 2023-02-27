#version 460 core
/**
 * @file   ssaoBlur.frag
 * @brief  Fragment shader for applying ssao-optimized blur to a texture.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform sampler2D ssaoTex;

in Vertex {
	vec2 texCoord;
} IN;

out float fragColour;

void main() {
	vec2 texelSize = 1.0 / vec2(textureSize(ssaoTex, 0));
	float result = 0.0;
	for (int x = -2; x < 2; x++) {
		for (int y = -2; y < 2; y++) {
			vec2 offset = vec2(float(x), float(y)) * texelSize;
			result += texture(ssaoTex, IN.texCoord + offset).r;
		}
	}
	fragColour = result / 16.0;
}
