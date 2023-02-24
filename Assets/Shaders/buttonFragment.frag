#version 460 core

uniform sampler2D diffuseTex;
//uniform sampler2D bumpTex;

in Vertex {
	vec2 texCoord;
	vec4 colour;
	vec3 normal;
	vec3 tangent;
	vec3 worldPos;
} IN ;

out vec4 fragColour;

void main ( void ) {
	fragColour = texture ( diffuseTex, IN.texCoord );
	fragColour.a = 1;
}
