varying vec3 baseColor;
uniform int cubeMapRes;
uniform float diffusePercent;
uniform samplerCube himmelCube;

varying vec3 normal;

vec3 o[2] = 
{ 
	vec3(0.297, 0.277, 0.130),
	vec3(0.93, 0.191, 0.84)
};

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

void main() {
	vec3 n = normalize(normal);
	
	vec3 diffuseColor = vec3(textureCube(himmelCube, n));
	vec3 hsv = rgb2hsv(vec4(diffuseColor, 1.0));
	
	
	
	//Add lighting to base color and mix
	vec3 color = mix(baseColor, diffuseColor, (1.0 - hsv.b) + hsv.g); //mix(, diffusePercent);
	
	gl_FragColor = vec4(color, 1.0);
}