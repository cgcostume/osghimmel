uniform vec3 baseColor;
uniform samplerCube himmelCube;
uniform sampler2D noiseMap;
uniform vec3 src;
uniform bool src_known;

varying vec3 normal;
varying vec3 pos;
varying float depth;

float minEnvAffection = 1.0;
float maxEnvAffection = 0.0;

vec3 kernel[32];

float dir_light_src(vec3 v) {
	float range = maxEnvAffection - minEnvAffection;
	float d = dot(normalize(v), normalize(src));
	d = (d + 1.0) / 2.0;
	return d * range + minEnvAffection;
}


//
//color space conversion
//
vec3 rgb2hsv(vec4 color)
{
    float h = 0.0, s = 0.0, v = 0.0;
	float colorMax, colorMin;

	colorMax = max(max(color.r,color.g), color.b);
    colorMin = min(min(color.r,color.g), color.b);
    v = colorMax;                // this is value
    
    if(colorMax != 0.0)
      s = (colorMax - colorMin ) / colorMax;
    
    if(s != 0.0)
    {
        float diff = colorMax - colorMin;
        
		if(color.r == colorMax) {
            h = (color.g - color.b) / diff;
        } else if(color.g == colorMax) {        
            h = 2.0 + (color.b - color.r) / diff;
        } else { //b is max
            h = 4.0 + (color.r - color.g) / diff;
        }
        
        h *= 60.0;
        if(h < 0.0)
			h += 360.0;
    }
	
    return vec3(h, s, v);
}

vec3 hsv2rgb(vec3 hsv)
{
	float h = hsv.r;
	float s = hsv.g;
	float v = hsv.b;
	float r, g, b;
	int i;
	float f, p, q, t;

	if( s == 0.0 ) {
		// achromatic (grey)
		r = g = b = v;
		return vec3(r, g, b);
	}

	h /= 60.0;			// sector 0 to 5
	i = int(floor( h ));
	f = h - float(i);			// factorial part of h
	p = v * ( 1.0 - s );
	q = v * ( 1.0 - s * f );
	t = v * ( 1.0 - s * ( 1.0 - f ) );

	switch( i ) {
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		default:		// case 5:
			r = v;
			g = p;
			b = q;
			break;
	}
	
	return vec3(r, g, b);

}

void init() {
	kernel[0] = vec3(-0.35059, 0.662226, 0.2226);
	kernel[1] = vec3(-0.912111, 0.326602, 0.2189907);
	kernel[2] = vec3(0.658323, 0.188092, 0.728857);
	kernel[3] = vec3(0.268081, -0.861688, 0.430844);
	kernel[4] = vec3(0.630132, -0.467517, 0.619969);
	kernel[5] = vec3(0.636674, 0.698788, 0.326101);
	kernel[6] = vec3(-0.435028, -0.2648, 0.860599);
	kernel[7] = vec3(-0.642728, -0.670673, 0.370267);
	kernel[8] = vec3(-0.35059, 0.662226, 0.662226);
	kernel[9] = vec3(-0.912111, 0.346602, 0.218907);
	kernel[10] = vec3(0.658323, 0.188092, 0.728857);
	kernel[11] = vec3(0.268081, -0.861688, 0.430844);
	kernel[12] = vec3(0.630132, -0.467517, 0.619969);
	kernel[13] = vec3(0.636674, 0.698788, 0.326101);
	kernel[14] = vec3(-0.435028, -0.2648, 0.860599);
	kernel[15] = vec3(-0.642728, -0.670673, 0.370267);
	kernel[16] = vec3(0.10083, 0.604978, 0.789832);
	kernel[17] = vec3(-0.190476, 0.380952, 0.904762);
	kernel[18] = vec3(-0.66492, 0.46033, 0.588199);
	kernel[19] = vec3(-0.199117, -0.464606, 0.86284);
	kernel[20] = vec3(-0.582581, -0.257021, 0.771064);
	kernel[21] = vec3(-0.205398, 0.975643, 0.0770244);
	kernel[22] = vec3(-0.290129, -0.232104, 0.928414);
	kernel[23] = vec3(-0.39036, -0.78072, 0.48795);
	kernel[24] = vec3(0.699926, -0.367461, 0.612435);
	kernel[25] = vec3(0, -0.999935, 0.0113629);
	kernel[26] = vec3(0.946657, -0.0440305, 0.319221);
	kernel[27] = vec3(-0.528094, 0.665007, 0.528094);
	kernel[28] = vec3(0.171429, -0.285714, 0.942857);
	kernel[29] = vec3(0.801234, -0.585517, 0.123267);
	kernel[30] = vec3(-0.332309, -0.609234, 0.720003);
	kernel[31] = vec3(-0.760941, -0.366379, 0.535477);
}

//
// Description : Array and textureless GLSL 2D/3D/4D simplex
// noise functions.
// Author : Ian McEwan, Ashima Arts.
// Maintainer : ijm
// Lastmod : 20110822 (ijm)
// License : Copyright (C) 2011 Ashima Arts. All rights reserved.
// Distributed under the MIT License. See LICENSE file.
// https://github.com/ashima/webgl-noise
//

/*vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
     return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v)
  {
  const vec2 C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4 D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i = floor(v + dot(v, C.yyy) );
  vec3 x0 = v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  // x0 = x0 - 0.0 + 0.0 * C.xxx;
  // x1 = x0 - i1 + 1.0 * C.xxx;
  // x2 = x0 - i2 + 2.0 * C.xxx;
  // x3 = x0 - 1.0 + 3.0 * C.xxx;
  vec3 x1 = x0 - i1 + C.xxx;
  vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
  vec3 x3 = x0 - D.yyy; // -1.0+3.0*C.x = -0.5 = -D.y

// Permutations
  i = mod289(i);
  vec4 p = permute( permute( permute(
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients: 7x7 points over a square, mapped onto an octahedron.
// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
  float n_ = 0.142857142857; // 1.0/7.0
  vec3 ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z * ns.z); // mod(p,7*7)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ ); // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
  //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
                                dot(p2,x2), dot(p3,x3) ) );
  }*/

void main() {
	init();
	
	vec3 v = normal;
	vec3 color = baseColor;
	
	//float nnNoise = snoise(vec3(normal.x, gl_FragCoord));
	//vec3 rvec = normalize(vec3(nnNoise, nnNoise, 0.0)) * 2.0 - 1.0;
	vec3 rvec = normalize(vec3(texture2D(noiseMap, gl_FragCoord.xy).rg, 0.0)) * 2.0 - 1.0;

	//tangent space conversion
	vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 tbn = mat3(tangent, bitangent, normal);
	
	vec3 envColor = vec3(1.0);
	float totalWeights = 0.0;
	
	for (int i = 0; i < 8; ++i) {
		vec3 noise = kernel[i];
		v = tbn * noise;
		
		vec3 vColor = vec3(textureCube(himmelCube, v));
		//vec3 envColorHsv = rgb2hsv(vec4(envColor, 1.0));
		
		float dirWeight = 1.0;
		if (src_known)
			dirWeight = pow(dir_light_src(v), 1.5);
			
		color = mix(vColor, color, dirWeight);
	}
	
	//color balance
	//float luminance = (baseColor.x + baseColor.y + baseColor.z) / 3.0;
	//color = mix(envColor, color, dir_light_src(normal) + (1.0 - luminance));
	
	//final fragment
	gl_FragColor = vec4(color, 1.0);
}