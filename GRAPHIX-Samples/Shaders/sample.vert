#version 330 core // Version number

layout(location = 0) in vec3 aPos; 

layout(location = 2) in vec2 aTex; // Texture here; Need to pass to fragment shader; Get vector 2 in index 2

out vec2 texCoord; // Output 

// REMOVE uniform float x;
uniform mat4 transform; // Transformation matrix for later
uniform mat4 projection;
uniform mat4 view;

void main(){
	//REMOVE vec3 newPos = vec3(aPos.x + x, aPos.y, aPos.z);
	//Converts it to vec4
	//Assigns it to a special variable called gl_Position
	//REMOVE gl_Position = vec4(newPos, 1.0); // Need to convert aPos into vec4
	gl_Position = projection * view * transform * vec4(aPos, 1.0); // Applies transform; Turns the vec3 into a vec4
																	// view must be IN BETWEEN projection and transform
	texCoord = aTex;
}