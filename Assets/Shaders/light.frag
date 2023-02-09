#version 460 core
/**
 * @file   light.frag
 * @brief  Fragment shader for deferred diffuse and specular lighting.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform sampler2D depthTex;
uniform sampler2D normalTex;
uniform sampler2D shadowTex;

uniform mat4 shadowMatrix;
uniform mat4 inverseProjView;

uniform vec2 pixelSize;
uniform vec3 cameraPos;

uniform vec4  lightPosition;
uniform float lightRadius;
uniform vec4  lightColour;
uniform vec3  lightDirection;
uniform float lightAngle;

uniform float gamma = 2.2;

out vec4 diffuseOutput;
out vec4 specularOutput;

void main() {
	vec2 texCoord = vec2(gl_FragCoord.xy * pixelSize);
	float depth   = texture(depthTex, texCoord).r;
	if (depth == 1.0)
		discard;
	vec3 ndcPos     = vec3(texCoord, depth) * 2.0 - 1.0;
	vec4 invClipPos = inverseProjView * vec4(ndcPos, 1.0);
	vec3 worldPos   = invClipPos.xyz / invClipPos.w;

	vec3 incident;
	float attenuation;
	if (lightPosition.w == 0) { // Directional Light
		incident = normalize(lightDirection);
		attenuation = 1.0;
	} else {                    // Point/Spot light
		vec3 lPos = lightPosition.xyz / lightPosition.w;
		incident = normalize(lPos - worldPos);
		if (lightAngle < 360.0 && degrees(acos(dot(-incident, lightDirection))) > lightAngle)
			discard;
		float dist = length(lPos - worldPos);
		if (dist > lightRadius)
			discard;
		attenuation = 1.0 / (gamma == 1.0 ? dist : dist * dist);
		if (attenuation <= 0.0)
			discard;
	}

	vec3 normal  = normalize(texture(normalTex, texCoord).xyz * 2.0 - 1.0).grb;
	vec3 viewDir = normalize(cameraPos - worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	vec4 pushVal = vec4(normal, 0.0) * dot(viewDir, normal) * 1.0;
	vec4 shadowProj = shadowMatrix * (vec4(worldPos, 1.0) + pushVal);

	float shadow = 1.0;
	vec3 shadowNDC = shadowProj.xyz / shadowProj.w;
	if (abs(shadowNDC.x) < 1.0 && abs(shadowNDC.y) < 1.0 && abs(shadowNDC.z) < 1.0) {
		vec3 biasCoord = shadowNDC * 0.5 + 0.5;
		float shadowZ = texture(shadowTex, biasCoord.xy).x;
		shadow = (shadowZ < biasCoord.z) ? 0.0 : 1.0;
	}

	float lambert = clamp(dot(incident, normal), 0.0, 1.0);
	float rFactor = clamp(dot(halfDir, normal),  0.0, 1.0);
	float specFactor = pow(rFactor, 60.0);

	vec3 attenuated = lightColour.xyz * attenuation;

	diffuseOutput  = vec4(attenuated * lambert * 1, 1.0);
	specularOutput = vec4(attenuated * specFactor * 0.33 * 1, 1.0);
}
