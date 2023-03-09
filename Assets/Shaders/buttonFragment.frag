#version 460 core

uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
	vec4 colour;
	vec3 normal;
	vec3 tangent;
	vec3 worldPos;
} IN ;

out vec4 diffuseOut;

void main ( void ) {
	diffuseOut = IN.colour;
	diffuseOut *= texture ( diffuseTex, IN.texCoord );
}
