#version 330 core

out vec4 FragColour;

uniform vec3 lightColour;
uniform vec3 objectColour;
void main(){
	FragColour = vec4(lightColour * objectColour, 1.0);
}
