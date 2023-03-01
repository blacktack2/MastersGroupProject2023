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
	fragColour = vec4(texture(diffuseTex, IN.texCoord).rgb, 1.0);
}
