#version 150
out vec4 oColor; 
in vec3 n;
uniform vec3 color;
void main() 
{
	oColor = vec4( dot( vec3(0,0,1),normalize(n)) * color, 1.0 );
}