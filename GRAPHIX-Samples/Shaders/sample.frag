#version 330 core //shader version
uniform sampler2D tex0;

in vec2 texCoord; // Receive the output variable for textures in sample.vert

out vec4 FragColor;


void main(){
	// FragColor = vec4(0.5f, 0f, 0f, 1f); // RGBA

	FragColor = texture(tex0, texCoord); // Assign the pixels, given our UV, to the model of our object; The wrapping part
}