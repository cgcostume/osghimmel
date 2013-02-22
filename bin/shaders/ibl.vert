uniform int cubeMapRes;
uniform float diffusePercent;
uniform samplerCube himmelCube;
//uniform vec3 kernel[8];

uniform vec3 src;
uniform bool src_known;

varying vec3 pos;
varying vec3 baseColor;
varying vec3 normal;

void main() {
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	normal = normalize(gl_Normal);
	baseColor = vec3(1.0, 1.0, 1.0);
	pos = gl_Vertex;
}