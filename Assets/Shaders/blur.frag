#version 460 core
/**
 * @file   blur.frag
 * @brief  Fragment shader for applying gaussian blur to a texture.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform sampler2D sceneTex;

uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.194594, 0.1216216, 0.054054, 0.016216);
 
in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour;

 void main() {
	vec2 texOffset = 1.0 / textureSize(sceneTex, 0);
	vec3 result = texture(sceneTex, IN.texCoord).rgb * weight[0];
	vec2 direction = horizontal ? vec2(texOffset.x, 0.0) : vec2(0.0, texOffset.y);
	for (int i = 1; i < 5; i++) {
		result += texture(sceneTex, IN.texCoord + direction * i).rgb * weight[i];
		result += texture(sceneTex, IN.texCoord - direction * i).rgb * weight[i];
	}
	fragColour = vec4(result, 1.0);
 }
