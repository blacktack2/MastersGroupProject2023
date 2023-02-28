#version 460 core
/**
 * @file   modelAnimated.frag
 * @brief  
 * 
 * @author 
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;

uniform vec4 modelColour;

uniform float gamma = 2.2;

in Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
} IN;

out vec4 diffuseOut;
out vec4 normalOut;

void main() {
	mat3 TBN = mat3(normalize(IN.tangent),
					normalize(IN.binormal),
					normalize(IN.normal));

	vec3 normal = texture(bumpTex, IN.texCoord).rgb * 2.0 - 1.0;
	normal = normalize(TBN * normalize(normal));

	diffuseOut = (texture(diffuseTex, IN.texCoord) + IN.colour) * modelColour;
	diffuseOut.rgb = pow(diffuseOut.rgb, vec3(gamma));

	normalOut = vec4(normal * 0.5 + 0.5, 1.0);
}