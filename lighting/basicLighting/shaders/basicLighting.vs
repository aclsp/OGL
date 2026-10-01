#version 330 core
in vec3 aPos;
in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));	
	Normal = aNormal;
	
	gl_Position = projection * view * model * vec4(aPos, 1.0);
 }
