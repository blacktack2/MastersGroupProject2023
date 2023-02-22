#version 330
uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
	//vec3 normal;
} IN;

out vec4 fragColour;

void main(void){
	fragColour = vec4(texture(diffuseTex, IN.texCoord).rgb,1.0);

	//fragColour.xyz = IN.normal;

//fragColour = IN.colours;
	//fragColour = vec4(IN.texCoord, 0, 1);
}