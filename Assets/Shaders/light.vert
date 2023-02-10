#version 460 core
/**
 * @file   light.vert
 * @brief  Vertex shader for deferred diffuse and specular lighting.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform float lightRadius;
uniform vec4 lightPosition;

layout(location = 0) in vec3 position;

void main () {
	if (lightPosition.w == 0.0) {  // Directional Light
		gl_Position = vec4(position, 1.0);
	} else {                       // Point/Spot Light
		vec3 scale = vec3(lightRadius);
		vec3 worldPos = (vec4(position * scale, 1.0) * modelMatrix).xyz + lightPosition.xyz;
		gl_Position = projMatrix * viewMatrix * vec4(worldPos, 1.0);
	}
}
