uniform vec3 baseColor;
uniform float diffusePercent;
uniform samplerCube himmelCube;

varying vec3 normal;

vec3 o[2] = 
	{ vec3(0.297, 0.277, 0.130)
	, vec3(0.93, 0.191, 0.84) 
	, vec3(0.93, 0.191, 0.84) 
	, vec3(0.93, 0.191, 0.84) 
	, vec3(0.93, 0.191, 0.84) 
	, vec3(0.93, 0.191, 0.84) 
	, vec3(0.93, 0.191, 0.84) 
	, vec3(0.93, 0.191, 0.84) 
	, vec3(0.93, 0.191, 0.84) 
	, vec3(0.93, 0.191, 0.84) };

void main() {
	//Look up environment map values in cube maps
	vec3 n = normalize(normal);
	
	vec3 diffuseColor = vec3(0.0);
	for(int i = 0; i < 2; ++i)
		diffuseColor += vec3(textureCube(himmelCube, n + (o[i] - vec3(0.5)) * 0.9 ));
		
	diffuseColor /= 2.0;
		
	float l = dot(n, normalize(vec3(0.0, 1.0, 1.0)));
	//Add lighting to base color and mix
	vec3 color = baseColor * diffuseColor; //mix(, diffusePercent);
	
	gl_FragColor = vec4(color, 1.0);
}