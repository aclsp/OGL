#version 330 core
out vec4 FragColour;
in vec3 ourPos;

uniform int maxIter;

vec3 hsv2rgb(vec3 c);

vec2 squareComplex(vec2 num){
	return vec2(num.x * num.x - num.y * num.y, 2 * num.y * num.x);
}
void main(){
	vec2 z = vec2(ourPos.x, ourPos.y);
	int i;
	for( i = 0; i < maxIter; i++){
		z = squareComplex(z) + vec2(ourPos.x, ourPos.y);
		if (length(z) > 2.0) break; 
	}
	if (length(z) > 2.0){
		float smooth_i = float(i - 1) - log2(log2(length(z))) + 4.0;
		float t = smooth_i / maxIter;
		vec3 color = hsv2rgb(vec3(t, 0.8, 1.0));
		FragColour = vec4(color, 1.0f);
	}
	else{
		FragColour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
}
vec3 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
