#version 460 core
/**
 * @file   modelDisplace.tese
 * @brief  Tesselation Evaluation shader for rendering model data to the
 * GBuffer with tesselation-based displacement mapping.
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

layout(triangles, fractional_odd_spacing, ccw) in;

uniform sampler2D heightTex;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform vec2 texScale = vec2(1.0);
uniform vec2 heightBounds = vec2(0.0, 5.0);

vec2 triangleLerp(vec2 a, vec2 b, vec2 c) {
	return a * gl_TessCoord.x + b * gl_TessCoord.y + c * gl_TessCoord.z;
}

vec3 triangleLerp(vec3 a, vec3 b, vec3 c) {
	return a * gl_TessCoord.x + b * gl_TessCoord.y + c * gl_TessCoord.z;
}

vec4 triangleLerp(vec4 a, vec4 b, vec4 c) {
	return a * gl_TessCoord.x + b * gl_TessCoord.y + c * gl_TessCoord.z;
}

in Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
} IN[];

out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} OUT;

void main() {
	OUT.colour   = triangleLerp(IN[0].colour, IN[1].colour, IN[2].colour);
	OUT.texCoord = triangleLerp(IN[0].texCoord, IN[1].texCoord, IN[2].texCoord);
	OUT.normal   = triangleLerp(IN[0].normal, IN[1].normal, IN[2].normal);
	OUT.tangent  = triangleLerp(IN[0].tangent, IN[1].tangent, IN[2].tangent);
	OUT.binormal = triangleLerp(IN[0].binormal, IN[1].binormal, IN[2].binormal);
	
	vec4 position = triangleLerp(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position);
	float height  = texture(heightTex, OUT.texCoord * texScale).r * heightBounds.y + heightBounds.x;
	vec4 worldPos = modelMatrix * (position) + vec4(OUT.normal * height, 0.0);
	OUT.worldPos  = worldPos.xyz;

	gl_Position = projMatrix * viewMatrix * worldPos;
}
