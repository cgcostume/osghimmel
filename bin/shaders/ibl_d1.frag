varying vec3 baseColor;
uniform int cubeMapRes;
uniform float diffusePercent;
uniform samplerCube himmelCube;
uniform sampler2D noiseMap;
//uniform vec3 kernel[8];
vec3 kernel[32];

varying vec3 normal;
varying vec3 pos;

//vec3 src = vec3(3.0, 5.0, -1.0);
uniform vec3 src;
//bool src_known = true;
uniform bool src_known;

float minEnvAffection = 0.2;
float maxEnvAffection = 0.9;

vec3 white = vec3(0.82, 0.82, 0.82);

//
//random vector picking
//
float rand()
{
  return 0.5 + 0.5 * 
     fract(sin(dot(pos.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

float U_m1_p1(){
  return float(rand())* (1.0/2147483648.0) - 1.0;
}

vec3 pick_random_point_in_sphere(){
	float r = rand();
	return pos + vec3(r, r, r);
}

vec3 pick_random_point_in_semisphere(vec3 n){
  vec3 result = pick_random_point_in_sphere();
  
  if(dot(n, result) < 0.0){
    return -1.0 * result;
  }

  return result;
}

float dir_light_src(vec3 v) {
	float range = maxEnvAffection - minEnvAffection;
	float d = dot(v, normalize(src));
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

/*vec4 hsv2rgb(vec3 hsv)
{
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    float f,p,q,t, h,s,v;
    float r = 0.0, g = 0.0, b = 0.0, i;
    
    if(hsv[1] == 0.0)
    {
        if(hsv[2] != 0.0)
        {
            color = vec4(hsv[2]);        // black and white case
        }
    }
    else
    {        // this next step is flawed if the texels are 8 bit!!
        h = hsv.x * 360.0;
        s = hsv.y;
        v = hsv.z;
        if(h == 360.0)
        {   
            h = 0.0;
        }

        h /= 60.0;
        i = floor(h);
        f = h-i;
        p = v * (1.0 - s);
        q = v * (1.0 -(s * f));
        t = v * (1.0 -(s * (1.0 -f)));

	   if( i == 0.0) {
	   	r = v;
		g  = t; 
		b = p; 
	   } else {
		  if (i== 1.0) {
			r = q;
			g = v;
			b = p;
		  } else {
			if (i==2.0) {
				r = p;
				g = v;
				b = t;
			} else {
				if (i==3.0) {
					r = p;
					g = q;
					b = v;
				} else {
					if (i==4.0) {
						r = t;
						g = p;
						b = v;
					} else {
						if (i==5.0) {
							r = v;
							g = p;
							b = q;
						}
					}
				}
			}
		 } 
	   }

        color.r = r;
	   color.g = g;
	   color.b = b;
    }
    return color;
}*/


vec3 hsv2rgb(vec3 hsv)
{
	float h = hsv.r;
	float s = hsv.g;
	float v = hsv.b;
	float r, g, b;
	int i;
	float f, p, q, t;

	if( s == 0 ) {
		// achromatic (grey)
		r = g = b = v;
		return vec3(r, g, b);
	}

	h /= 60;			// sector 0 to 5
	i = floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

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

void main() {
	init();
	
	vec3 v = normal;
	vec3 color = baseColor;
	
	vec3 rvec = normalize(vec3(pos.xy, 0.0)) * 2.0 - 1.0;
	
	vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 tbn = mat3(tangent, bitangent, normal);
	
	//vec3 test = vec3(0.0);
	for (float i = 0.0; i < 8.0; ++i) {
		if (i > 0.0) {
			vec2 uv = gl_FragCoord.xy * 1.0;
			vec3 noise = normalize(texture2D(noiseMap, vec2(uv.x + i * 1.0/32.0, uv.y)));
			v = tbn * noise;	

		}
		
		v = mix(v, normal, 0.0);
		
		vec3 envColor = vec3(textureCube(himmelCube, v));
		vec3 envColorHsv = rgb2hsv(vec4(envColor, 1.0));
		
	///	test += envColor * 0.125;
		
	
		float dirWeight = 1.0;
		if (src_known)
			dirWeight = dir_light_src(normal);
			
		//vec3 colorHsv = rgb2hsv(vec4(color, 1.0));
		//color = hsv2rgb( vec3(colorHsv.r, colorHsv.g, envColorHsv.b * dirWeight + (1.0 - dirWeight) * colorHsv.b) );
		color = mix(color, envColor, dirWeight / (i + 1.0));
		
	}
	
		//								gl_FragColor = vec4( test, 1.0);
	//return;

	
	
	//gl_FragColor = texture2D(noiseMap, gl_TexCoord[0]);
	gl_FragColor = vec4(color, 1.0);
	
}