#version 460 core
/**
 * @file   bloomCombine.frag
 * @brief  Fragment shader for adding bloom to a texture.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

 uniform sampler2D sceneTex;
 uniform sampler2D bloomTex;
 
in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

 void main() {
	vec3 sceneColour = texture(sceneTex, IN.texCoord).rgb;
	vec3 bloomColour = texture(bloomTex, IN.texCoord).rgb;
	sceneColour += bloomColour;

	fragColour = vec4(sceneColour, 1.0);
 }
