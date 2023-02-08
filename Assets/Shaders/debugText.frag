#version 460 core
/**
 * @file   debugText.frag
 * @brief  Fragment shader for drawing debug text.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform sampler2D fontTex;

in Vertex {
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main() {
	float alpha = texture(fontTex, IN.texCoord).r;
		
	if(alpha < 0.00001f)
		discard;
		
	fragColour = IN.colour * vec4(vec3(1), alpha);
}