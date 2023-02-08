#version 460 core
/**
 * @file   debugLines.frag
 * @brief  Fragment shader for drawing debug lines.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */

in Vertex {
	vec4 colour;
} IN;

out vec4 fragColour;

void main() {
	fragColour = IN.colour;
}