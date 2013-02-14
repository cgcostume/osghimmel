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

int warp3(int x) {
	float a = pow(cubeMapRes, 2) / pow((cubeMapRes-1), 3);
	return a * pow(x, 3) + x;
}

/*float map_1(int x) {
	return (warp3(x) + 0.5) * 2 / cubeMapRes - 1;
}*/

float map_1(int x) {
	return (x + 0.5) * 2 / cubeMapRes - 1;
}

float map_2(int x) {
	return 2 * warp3(x) / (cubeMapRes - 1) - 1;
}

void main() {
	//Look up environment map values in cube maps
	vec3 n = normalize(normal);
	//vec3 n = normalize(normal + normal.x * map_2(normal.x) + normal.y * map_2(normal.y));
	
	//vec3 diffuseColor = textureCube(himmelCube, normalize(normal));
	
	/*vec3 diffuseColor = vec3(0.0);
	for(int i = 0; i < 2; ++i) {
		//vec3 m = normal + (o[i] - vec3(0.5)) * 0.3;
		//diffuseColor = textureCube(himmelCube, normalize(m + m.s * map_2(m.s) + m.t * map_2(m.t)) );
		//diffuseColor += normalize(faceVec + faceVec.s * map_2(faceVec.s) + faceVec.t * map_2(faceVec.t));
		diffuseColor += textureCube(himmelCube, n + (o[i] - vec3(0.5)) * 0.3);
	}*/
	
	vec3 diffuseColor = textureCube(himmelCube, n);
		
		
		
	//diffuseColor *= diffusePercent;
	
	//float l = dot(n, normalize(vec3(0.0, 1.0, 1.0)));
	//Add lighting to base color and mix
	vec3 color = baseColor * diffuseColor; //mix(, diffusePercent);
	
	gl_FragColor = vec4(diffuseColor, 1.0);
}