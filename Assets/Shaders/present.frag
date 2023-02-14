#version 460 core
/**
 * @file   present.frag
 * @brief  Fragment shader for rendering the final image to screen.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform sampler2D sceneTex;

uniform float gamma = 2.2;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main() {
	vec3 scene = texture(sceneTex, IN.texCoord).rgb;
	fragColour = vec4(pow(scene, vec3(1.0 / gamma)), 1.0);
}
