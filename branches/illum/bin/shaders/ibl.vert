uniform vec3 baseColor;
uniform float diffusePercent;
uniform samplerCube DiffuseEnvMap;

varying vec3 normal;

void main() {
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	normal = normalize(gl_NormalMatrix * gl_Normal);
}