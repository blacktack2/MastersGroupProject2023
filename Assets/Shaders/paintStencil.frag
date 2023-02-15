#version 460 core

uniform vec3 paintPos;
uniform float paintSize;

in vertex {
	vec3 worldPos;
} IN;

out vec4 fragColour;
out vec4 worldPos;

void main() {
	if(IN.worldPos.y < 0) return;
	vec3 relative = IN.worldPos - paintPos;
	float d = dot(relative, relative);
	if(d <= (paintSize * paintSize)) {fragColour = vec4(1,1,1,1);}
	worldPos.rgb = abs(IN.worldPos / 200);
	worldPos.a = 1;
}