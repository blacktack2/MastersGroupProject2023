#version 460 core
/**
 * @file loading.frag
 * @brief
 *
 * @author Harry Brettell
 * @date March 2023
 */

uniform float time;
uniform vec2 screenSize;


out vec4 fragColour;

void main() {
	vec2 scaledCoord = gl_FragCoord.xy - screenSize/2;
	
	float radius = dot(scaledCoord, scaledCoord);

	if(radius <= 105.0) {
		fragColour = vec4(1,1,1,1);
	} else {
		fragColour = vec4(0,0,0,1);
	}

}