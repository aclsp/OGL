#version 330 core
in vec3 aPos;
in vec2 aTexCoord;

out vec2 ourTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0f);
	ourTexCoord = aTexCoord;
 }
