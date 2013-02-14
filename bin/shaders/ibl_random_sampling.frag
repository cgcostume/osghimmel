varying vec3 baseColor;
uniform int cubeMapRes;
uniform float diffusePercent;
uniform samplerCube himmelCube;

varying vec3 normal;
varying vec3 pos;

vec3 src = vec3(3.0, 5.0, -1.0);
bool src_known = true;

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

void main() {
	//vec3 baseColorHsv = rgb2hsv(vec4(baseColor, 1.0));
	
	//vec3 color = baseColor;
	//for (float i = 0.0; i < 5.0; ++i) {
		
		vec3 v = normal;
		//if (i > 0.0)
		//v = normalize(pick_random_point_in_semisphere(normal));
		vec3 color = baseColor;
		
		for (float i = 0.0; i < 5.0; ++i) {
		if (i > 0.0)
			v = pick_random_point_in_semisphere(normal);
		
		vec3 envColor = vec3(textureCube(himmelCube, v));
		//vec3 neutral = vec3(1.0/white.r * envColor.r, 1.0/white.g * envColor.g, 1.0/white.b * envColor.b);
		
		vec3 envColorHsv = rgb2hsv(vec4(envColor, 1.0));
		vec3 brightness = envColorHsv.b;
		
		float dirWeight = 1.0;
		if (src_known)
			dirWeight = dir_light_src(normal);
			//color = mix(baseColor, color, dirWeight);
			
		color = mix(color, envColor, dirWeight/(i+1.0));
		}
			
		//color = vec3(envColor.r * baseColor.r, envColor.g * baseColor.g, envColor.b * baseColor.b);			
			
		//vec3 colorHsv = rgb2hsv(vec4(color, 1.0));
		
		//vec3 newColor = vec3(colorHsv.r, colorHsv.g, envColorHsv.b * 0.8);
		//color = hsv2rgb(newColor);
		
		//vec3 diff = envColor - white;
		
		//float inS = envColorHsv.g; //influence of environment saturation
		//float inB = envColorHsv.b; //influence of environment brightness

		//float weight = 1.0;
		//if (src_known)
		//	weight = dir_light_src(v);

		//color = mix(envColor, color, inB * inS * weight);
		//color = mix(color, diff, 1.0);
	//}
	
	
	gl_FragColor = vec4(color, 1.0);
}