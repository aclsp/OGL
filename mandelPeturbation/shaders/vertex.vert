#version 430 core 
in vec3 aPos;
out vec3 ourPos;

void main(){
	ourPos = aPos;
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
 }
