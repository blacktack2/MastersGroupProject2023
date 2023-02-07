#version 460 core
/**
 * @file   present.frag
 * @brief  Fragment shader for rendering the final image to screen.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

 uniform sampler2D sceneTex;

 in Vertex {
	vec2 texCoord;
 } IN;

 out vec4 fragColour;

 void main() {
	fragColour = texture(sceneTex, IN.texCoord);
 }
