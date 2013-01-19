#version 150
in vec4 aPosition;
out vec3 n;
uniform mat4 modelView; 
uniform mat4 projection;
void main() {
	gl_Position = projection*modelView*aPosition;
	n = (transpose(inverse(mat3(modelView)))*aPosition.xyz);
}