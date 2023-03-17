#version 460 core
/**
 * @file   modelDefault.frag
 * @brief  Default fragment shader for rendering model data to the GBuffer.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform sampler2D albedoTex;
uniform sampler2D bumpTex;
uniform sampler2D specTex;
uniform sampler2D paintTex;

uniform vec4 modelColour;

uniform vec2 texScale = vec2(1.0);

uniform float gamma = 2.2;

in Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 albedoOut;
out vec4 normalOut;
out vec4 specOut;

void main() {
	vec4 paint = texture(paintTex, IN.texCoord);
	paint.a = (paint.a > 0.5) ? 0.96 : 0.0;

	albedoOut = (texture(albedoTex, IN.texCoord * texScale) + IN.colour) * modelColour;
	albedoOut.rgb = pow(albedoOut.rgb, vec3(gamma));
	albedoOut.rgb = mix(albedoOut.rgb, paint.rgb, paint.a);

	mat3 TBN = mat3(normalize(IN.tangent),
					normalize(IN.binormal),
					normalize(IN.normal));

	vec3 normal = texture(bumpTex, IN.texCoord * texScale).rgb * 2.0 - 1.0;
	normal = normalize(TBN * normalize(normal));

	normalOut = vec4(normal * 0.5 + 0.5, 1.0);

	specOut = texture(specTex, IN.texCoord * texScale);
}
