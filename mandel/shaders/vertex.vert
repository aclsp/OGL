#version 330 core 
in vec3 aPos;
out vec3 ourPos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
void main(){
	ourPos = aPos;
	gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
 }
