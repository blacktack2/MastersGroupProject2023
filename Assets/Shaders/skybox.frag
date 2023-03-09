#version 460 core
/**
 * @file   skybox.frag
 * @brief  Fragment shader for a realistic procedural skybox with configurable
 * atmosphere and simplex-noise based clouds.
 * Procedural skybox adapted from (https://github.com/shff/opengl_sky).
 * Simplex noise adapted from (https://stegu.github.io/webgl-noise/webdemo/).
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */

uniform vec3 sunDir;

uniform float time;
uniform float cirrus;
uniform float cumulus;

const float Br = 0.0025;
const float Bm = 0.0003;
const float g  = 0.98;
const vec3 nitrogen = vec3(0.650, 0.570, 0.475);
const vec3 Kr = Br / pow(nitrogen, vec3(4.0));
const vec3 Km = Bm / pow(nitrogen, vec3(0.84));

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
     return mod289(((x * 34.0) + 10.0) * x);
}

vec4 taylorInvSqrt(vec4 r) {
  return 1.79284291400159 - 0.85373472095314 * r;
}

float simplex(vec3 v) { 
	const vec2 C = vec2(1.0 / 6.0, 1.0 / 3.0);
	const vec4 D = vec4(0.0, 0.5, 1.0, 2.0);

	vec3 i = floor(v + dot(v, C.yyy));
	vec3 x0 = v - i + dot(i, C.xxx);

	vec3 g = step(x0.yzx, x0.xyz);
	vec3 l = 1.0 - g;
	vec3 i1 = min(g.xyz, l.zxy);
	vec3 i2 = max(g.xyz, l.zxy);

	vec3 x1 = x0 - i1 + C.xxx;
	vec3 x2 = x0 - i2 + C.yyy;
	vec3 x3 = x0 - D.yyy;

	i = mod289(i); 
	vec4 p = permute(permute(permute(
		  i.z + vec4(0.0, i1.z, i2.z, 1.0))
		+ i.y + vec4(0.0, i1.y, i2.y, 1.0)) 
		+ i.x + vec4(0.0, i1.x, i2.x, 1.0));

	float n_ = 0.142857142857;
	vec3 ns = n_ * D.wyz - D.xzx;

	vec4 j = p - 49.0 * floor(p * ns.z * ns.z);

	vec4 x_ = floor(j * ns.z);
	vec4 y_ = floor(j - 7.0 * x_);

	vec4 x = x_ * ns.x + ns.yyyy;
	vec4 y = y_ * ns.x + ns.yyyy;
	vec4 h = 1.0 - abs(x) - abs(y);

	vec4 b0 = vec4(x.xy, y.xy);
	vec4 b1 = vec4(x.zw, y.zw);

	vec4 s0 = floor(b0) * 2.0 + 1.0;
	vec4 s1 = floor(b1) * 2.0 + 1.0;
	vec4 sh = -step(h, vec4(0.0));

	vec4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
	vec4 a1 = b1.xzyw + s1.xzyw * sh.zzww;

	vec3 p0 = vec3(a0.xy, h.x);
	vec3 p1 = vec3(a0.zw, h.y);
	vec3 p2 = vec3(a1.xy, h.z);
	vec3 p3 = vec3(a1.zw, h.w);

	vec4 norm = taylorInvSqrt(vec4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3)));
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;

	vec4 m = max(0.5 - vec4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0);
	m = m * m;
	return 0.5 + 52.5 * dot(m * m, vec4(dot(p0, x0), dot(p1, x1), dot(p2, x2), dot(p3, x3)));
}

float fBm(vec3 pos) {
	float f = simplex(pos       ) *  0.5
			+ simplex(pos * 2.0 ) *  0.25
			+ simplex(pos * 4.0 ) * (1.0 / 6.0)
			+ simplex(pos * 8.0 ) * (1.0 / 12.0)
			+ simplex(pos * 16.0) * (1.0 / 24.0);
	return f;
}

in Vertex {
	vec3 viewDir;
} IN;

out vec4 fragColour;

void main() {
	vec3 view = IN.viewDir;
	vec3 sun = -sunDir;
	view.y = abs(view.y);
	float mu = dot(normalize(view), -normalize(sun));
	float rayleigh = 3.0 / (8.0 * 3.1415926535) * (1.0 + mu * mu);
	vec3 mie = (Kr + Km * (1.0 - g * g) / (2.0 + g * g) / pow(1.0 + g * g - 2.0 * g * mu, 1.5)) / (Br + Bm);
	
	vec3 day_extinction =
		exp(-exp(-((view.y + sun.y * 4.0) * (exp(-view.y * 16.0) * 0.1) / 80.0) / Br) * (exp(-view.y * 16.0) + 0.1) * Kr / Br) *
		exp(-view.y * exp(-view.y * 8.0) * 4.0) *
		exp(-view.y * 2.0) * 4.0;
	vec3 night_extinction = vec3(1.0 - exp(sun.y)) * 0.2;
	vec3 extinction = mix(day_extinction, night_extinction, -sun.y * 0.2 + 0.5);

	fragColour.rgb = vec3(rayleigh * mie * extinction);
	
	if (IN.viewDir.y > 0.0) {
		float cirrusDensity = smoothstep(1.0 - cirrus, 1.0, fBm(view / view.y * 2.0 + time * 0.05)) * 0.3;
		fragColour.rgb = mix(fragColour.rgb, extinction * 4.0, cirrusDensity * view.y);

		for (int i = 0; i < 10; i++) {
			float cumulusDensity = smoothstep(1.0 - cumulus, 1.0, fBm((0.7 + float(i) * 0.01) * view / view.y + time * 0.3));
			fragColour.rgb = mix(fragColour.rgb, extinction * cumulusDensity * 5.0, min(cumulusDensity, 1.0) * view.y);
		}
	}

	fragColour.rgb += simplex(view * 1000) * 0.01;
	fragColour.w = 1.0;
}
