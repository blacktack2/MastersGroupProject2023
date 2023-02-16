#version 460 core

uniform vec3 paintPos;
uniform float paintSize;

in vertex {
	vec3 worldPos;
} IN;

out vec4 fragColour;

void main() {
	vec3 relative = IN.worldPos - paintPos;
	float d = dot(relative, relative);
	if(d <= (paintSize * paintSize)) {fragColour = vec4(1,1,1,1);}
}