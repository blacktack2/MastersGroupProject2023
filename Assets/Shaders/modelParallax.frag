#version 460 core
/**
 * @file   modelParallax.frag
 * @brief  Default fragment shader for rendering model data to the GBuffer,
 * with parallax occlusion mapping applied.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform sampler2D albedoTex;
uniform sampler2D bumpTex;
uniform sampler2D specTex;
uniform sampler2D paintTex;
uniform sampler2D parallaxTex;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

uniform vec4 modelColour;

uniform float heightScale = 0.05;

uniform vec2 texScale = vec2(1.0);

uniform float gamma = 2.2;

vec2 parallaxMap(vec2 texCoords, vec3 viewDir) {
	const float minLayers = 8.0f;
	const float maxLayers = 32.0f;
	float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));
	float layerDepth = 1.0 / numLayers;
	float currentLayerDepth = 0.0;

	vec2 p = viewDir.xy * heightScale;
	vec2 delta = p / numLayers;

	vec2 currentUV = texCoords;
	float currentHeight = texture(parallaxTex, currentUV).r;

	while (currentLayerDepth < currentHeight) {
		currentUV -= delta;
		currentHeight = texture(parallaxTex, currentUV).r;
		currentLayerDepth += layerDepth;
	}

	vec2 previousUV = currentUV + delta;
	float afterDepth = currentHeight - currentLayerDepth;
	float beforeDepth = texture(parallaxTex, previousUV).r - currentLayerDepth + layerDepth;

	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalUV = previousUV * weight + currentUV * (1.0 - weight);
	return finalUV;
}

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
	mat3 TBN = mat3(normalize(IN.tangent),
					normalize(IN.binormal),
					normalize(IN.normal));

	vec3 viewPos = inverse(viewMatrix)[3].xyz;
	vec3 viewDir = normalize(TBN * (viewPos - IN.worldPos)) * vec3(-1, 1, 1);
	vec2 texCoord = parallaxMap(IN.texCoord * texScale, viewDir);

	vec4 paint = texture(paintTex, texCoord / texScale);
	paint.a = (paint.a > 0.5) ? 0.96 : 0.0;

	albedoOut = (texture(albedoTex, texCoord) + IN.colour) * modelColour;
	albedoOut.rgb = pow(albedoOut.rgb, vec3(gamma));
	albedoOut.rgb = mix(albedoOut.rgb, paint.rgb, paint.a);

	vec3 normal = texture(bumpTex, texCoord).rgb * 2.0 - 1.0;
	normal = normalize(TBN * normalize(normal));

	normalOut = vec4(normal * 0.5 + 0.5, 1.0);

	specOut = texture(specTex, texCoord);
}
