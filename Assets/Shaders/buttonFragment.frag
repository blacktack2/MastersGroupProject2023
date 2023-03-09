#version 460 core

uniform sampler2D diffuseTex;
uniform sampler2D selectedTex;

in Vertex {
	vec2 texCoord;
	vec4 colour;
	vec3 normal;
	vec3 tangent;
	vec3 worldPos;
} IN ;

out vec4 diffuseOut;
out vec4 selectedOut;

void main ( void ) {
	diffuseOut = IN.colour;
	diffuseOut *= texture ( diffuseTex, IN.texCoord );

	selectedOut = IN.colour;
	selectedOut *= texture ( selectedTex, IN.texCoord );
}
