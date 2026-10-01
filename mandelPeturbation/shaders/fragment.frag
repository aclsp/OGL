#version 430 core
out vec4 FragColour;
in vec3 ourPos;

layout(std430, binding = 0) buffer OrbitBuffer {
	vec2 orbit[];
};

uniform int maxIter;
uniform vec2 center;
uniform float zoom;

vec3 hsv2rgb(vec3 c);

vec2 mulComplex(vec2 num, vec2 num2){
	return vec2(num.x * num2.x - num.y * num2.y, num.x * num2.y + num.y * num2.x);
}
void main(){
	vec2 point = vec2(0.0);
	vec2 c = vec2(ourPos.x * 3.5 * zoom, ourPos.y * 3.5 * zoom) + center;
	int i;
	vec2 delta = c - center;
	vec2 epsilon = vec2(0.0);
	
	for( i = 0; i < maxIter; i++){
		vec2 centerOrbit = orbit[i];
		point = centerOrbit + epsilon;	
	
		if (length(point) > 2.0) break; 
	epsilon = mulComplex(2.0 * centerOrbit, epsilon) + mulComplex(epsilon, epsilon) +  delta;
	}
	if (length(point) > 2.0){
		float smooth_i = float(i - 1) - log2(log2(length(point))) + 4.0;
		float t = smooth_i / maxIter;
		vec3 color = hsv2rgb(vec3(t, 0.8, 1.0));
		FragColour = vec4(color, 1.0);
	}
	else{
		FragColour = vec4(0.0f, 0.0f, 0.0f, 1.0);
	}
}
vec3 hsv2rgb(vec3 c){
    vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
