#version 330
uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
	//vec3 normal;
} IN;

out vec4 fragColour;

void main(void){
	fragColour = texture(diffuseTex, IN.texCoord);

	//fragColour.xyz = IN.normal;

//fragColour = IN.colours;
	//fragColour = vec4(IN.texCoord, 0, 1);
}