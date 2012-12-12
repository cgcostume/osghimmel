uniform vec3 baseColor;
uniform float diffusePercent;
uniform samplerCube himmelCube;

varying vec3 normal;

void main() {
	//Look up environment map values in cube maps
	vec3 diffuseColor = vec3(textureCube(himmelCube, normalize(normal)));
	
	//Add lighting to base color and mix
	vec3 color = mix(baseColor, diffuseColor * baseColor, diffusePercent);
	
	gl_FragColor = vec4(color, 1.0);
}