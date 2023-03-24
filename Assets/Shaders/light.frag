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
uniform sampler2D specTex;
uniform sampler2DShadow shadowTex;

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

void AttenuateDirLight(inout vec3 incident, inout float attenuation, vec3 lightDir) {
	incident = normalize(lightDir);
	attenuation = 1.0;
}
void AttenuateLight(inout vec3 incident, inout float attenuation, vec3 worldPos,
					vec3 lightPos, vec3 lightDirection, float lightRadius, float lightAngle) {
	incident = normalize(lightPos - worldPos);
	if (lightAngle > 0.0 && degrees(acos(dot(-incident, lightDirection))) > lightAngle)
		discard;
	vec3 delta = lightPos - worldPos;
	float dist2 = dot(delta, delta);
	if (dist2 > lightRadius * lightRadius)
		discard;
	attenuation = 1.0 / (gamma == 1.0 ? sqrt(dist2) : dist2);
	if (attenuation <= 0.0)
		discard;
}

void GetShadow(inout float shadow, vec3 worldPos) {
	vec3 shadowPos = (shadowMatrix * vec4(worldPos, 1.0)).xyz * 0.5 + 0.5;
	shadowPos.z -= 0.0005;
	shadow = texture(shadowTex, shadowPos).r;
}

out vec4 diffuseOutput;
out vec4 specularOutput;

void main() {
	vec2 texCoord = vec2(gl_FragCoord.xy * pixelSize);
	float depth   = texture(depthTex, texCoord).r;
	if (depth == 1.0) // Ignore skybox
		discard;
	vec3 ndcPos     = vec3(texCoord, depth) * 2.0 - 1.0;
	vec4 invClipPos = inverseProjView * vec4(ndcPos, 1.0);
	vec3 worldPos   = invClipPos.xyz / invClipPos.w;

	vec3 incident = vec3(0);
	float attenuation = 0;
	if (lightPosition.w == 0)
		AttenuateDirLight(incident, attenuation, lightDirection);
	else
		AttenuateLight(incident, attenuation, worldPos, lightPosition.xyz / lightPosition.w, lightDirection, lightRadius, lightAngle);

	vec3 normal  = normalize(texture(normalTex, texCoord).xyz * 2.0 - 1.0);
	vec3 viewDir = normalize(cameraPos - worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	float shadow = 0;
	GetShadow(shadow, worldPos);

	float lambert = clamp(dot(incident, normal), 0.0, 1.0);
	float rFactor = clamp(dot(halfDir, normal),  0.0, 1.0);
	float specFactor = pow(rFactor, 60.0);

	vec3 attenuated = lightColour.xyz * attenuation * shadow;

	diffuseOutput  = vec4(attenuated * lambert, 1.0);
	specularOutput = vec4(attenuated * specFactor * 0.33, 1.0) * texture(specTex, texCoord);
}
