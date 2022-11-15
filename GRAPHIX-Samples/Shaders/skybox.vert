#version 330 core

layout (location = 0) in vec3 aPos;

/* As opposed to vec2 since we are using a Cubemap */
out vec3 texCoords; 

uniform mat4 projection;
uniform mat4 view;

void main() {
	/* Notice how there is no transform - there is no need since this is the skybox */
	vec4 pos = projection * view * vec4(aPos, 1.0);

	/* write into view space - also note that there are 2 position w's */
	gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);

	texCoords = aPos;
}