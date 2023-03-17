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

float invLerp(float a, float b, float value) {
	return (value-a) / (b-a);
}

void main() {
	const float pi = 3.14159254;
	const float twoPi = 2.0 * pi;

	float beforeAngle = time * twoPi;
	float afterAngle = beforeAngle - 2;

	vec2 scaledCoord = gl_FragCoord.xy - screenSize/2;
	
	float angle = atan(scaledCoord.x, scaledCoord.y) + pi;
	float negAngle = angle - twoPi;

	float range = invLerp(afterAngle, beforeAngle, angle);
	if(range > 1 || range < 0) {
		range = invLerp(afterAngle, beforeAngle, negAngle);
	}
	if(range > 1) range = 0;

	vec4 colour = vec4(1) * range;

	float radius = dot(scaledCoord, scaledCoord);

	if(radius <= 10000.0 && radius >= 8000) {
		fragColour = colour;
	} else {
		fragColour = vec4(0,0,0,1);
	}

}