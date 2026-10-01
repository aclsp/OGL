#version 330 core

out vec4 FragColour;

in vec3 ourColour;
in vec2 ourTexCoord;

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
void main(){
	FragColour = mix(texture(ourTexture1, ourTexCoord) * vec4( ourColour, 1.0), texture(ourTexture2, ourTexCoord), 0.2);
}
