#version 330 core // Version number

layout(location = 0) in vec3 aPos; 

uniform float x;

void main(){
	vec3 newPos = vec3(aPos.x + x, aPos.y, aPos.z);

	//Converts it to vec4
	//Assigns it to a special variable called gl_Position
	gl_Position = vec4(newPos, 1.0); // Need to convert aPos into vec4
}