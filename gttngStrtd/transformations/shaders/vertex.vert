#version 330 core
in vec3 aPos;
in vec3 aColour;
in vec2 aTexCoord;

out vec3 ourColour;
out vec2 ourTexCoord;

uniform mat4 transform;

void main(){
	gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0f);
	ourColour = aColour;
	ourTexCoord = aTexCoord;
 }
