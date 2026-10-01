#version 330 core
in vec3 aPos;
in vec3 aColour;
out vec3 ourColour;
void main(){
	gl_Position = vec4(aPos.x, -1*aPos.y, aPos.z, 1.0);
	ourColour = aColour;
 }
