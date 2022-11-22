#version 330 core // Version number

layout(location = 0) in vec3 aPos; 
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 aTex; // Texture here; Need to pass to fragment shader; Get vector 2 in index 2
layout(location = 3) in vec3 m_tan;
layout(location = 4) in vec3 m_btan;

out mat3 TBN; // matrix

out vec2 texCoord; // Output 
out vec3 normCoord;
out vec3 fragPos;

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

	mat3 modelMat = mat3(transpose(inverse(transform)));

	normCoord = modelMat * vertexNormal;

	vec3 T = normalize(modelMat * m_tan);
	vec3 B = normalize(modelMat * m_btan);
	vec3 N = normalize(normCoord);

	TBN = mat3(T, B, N);

	fragPos = vec3(transform * vec4(aPos, 1.0));
}