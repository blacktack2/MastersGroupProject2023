#version 460 core
/**
 * @file   bloomCombine.frag
 * @brief  Fragment shader for applying bloom to a texture.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform sampler2D sceneTex;
uniform sampler2D bloomTex;

uniform float bias = 0.04;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

 void main() {
	vec3 sceneColour = texture(sceneTex, IN.texCoord).rgb;
	vec3 bloomColour = texture(bloomTex, IN.texCoord).rgb;

	fragColour = vec4(sceneColour + (bloomColour * bias), 1.0);
 }
