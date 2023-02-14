#version 460 core
/**
 * @file   bloomFilter.frag
 * @brief  Fragment shader for filtering out bright areas of a texture.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform sampler2D sceneTex;

uniform float threshold = 0.8;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main() {
	vec3 scene = texture(sceneTex, IN.texCoord).rgb;
	float brightness = dot(scene, vec3(0.2126, 0.7152, 0.0722));
	fragColour = brightness > threshold ? vec4(scene, 1.0) : vec4(vec3(0.0), 1.0);
}
