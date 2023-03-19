#version 460 core
/**
 * @file   menuFragment.frag
 * @brief  
 * 
 * @author 
 * @date   February 2023
 */

uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main() {
	vec4 colour = texture(diffuseTex, IN.texCoord);
	if (colour.a > 0.0)
		fragColour = vec4(colour.rgb, 1.0);
	else
		discard;
}
