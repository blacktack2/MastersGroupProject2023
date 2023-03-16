#version 460 core
/**
 * @file   modelPaintTexture.frag
 * @brief  Fragment shader for rendering model data with a paint map to the GBuffer.
 * 
 * @author Harry Brettell
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
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

out vec4 diffuseOut;
out vec4 normalOut;

void main() {
	mat3 TBN = mat3(normalize(IN.tangent),
					normalize(IN.binormal),
					normalize(IN.normal));

	vec4 paint = texture(paintTex, IN.texCoord);
	if(paint.a > 0.5) {
		paint.a = 0.96;
	}
	else {
		paint.a = 0;
	}
	
	vec3 normal = texture(bumpTex, IN.texCoord * texScale).rgb * 2.0 - 1.0;
	normal = normalize(TBN * normalize(normal));

	diffuseOut = (texture(diffuseTex, IN.texCoord * texScale) + IN.colour) * modelColour;
	diffuseOut.rgb = pow(diffuseOut.rgb, vec3(gamma));

	normalOut = vec4(normal * 0.5 + 0.5, 1.0);
	
	diffuseOut.rgb = paint.rgb * paint.a + diffuseOut.rgb * (1 - paint.a);
}
