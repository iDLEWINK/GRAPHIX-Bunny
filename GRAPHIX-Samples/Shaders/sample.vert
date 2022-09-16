#version 330 core // Version number

layout(location = 0) in vec3 aPos; 

void main(){
	//Converts it to vec4
	//Assigns it to a special variable called gl_Position
	gl_Position = vec4(aPos, 1.0); // Need to convert aPos into vec4
}