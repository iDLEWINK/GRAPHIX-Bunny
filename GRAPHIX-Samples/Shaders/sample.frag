#version 330 core //shader version

uniform sampler2D tex0;

uniform vec3 lightPos; // light source
uniform vec3 lightColor;

uniform float ambientStr; // ambient intensity or strength
uniform vec3 ambientColor;

uniform vec3 cameraPos;
uniform float specStr;
uniform float specPhong;

in vec2 texCoord; // Receive the output variable for textures in sample.vert
in vec3 normCoord;
in vec3 fragPos;

out vec4 FragColor;


void main(){
	// FragColor = vec4(0.5f, 0f, 0f, 1f); // RGBA

	// Current pixel color
	vec4 pixelColor = texture(tex0, texCoord);
	// Alpha cut off - every pixel below 0.1 would be discarded.
	if(pixelColor.a < 0.1) {
		discard; // akin to that of return or break; everything below is ignored
	}

	vec3 normal = normalize(normCoord);
	vec3 lightDir = normalize(lightPos - fragPos); // Direction from the light source to your fragment source

	/* DIFFUSE */
	float diff = max(dot(normal, lightDir), 0.0f); // Max so we do not have any negative lights
	// vec3 diffuse = diffuseIntensity * diff * lightColor; // you can add diffuseIntensity
	vec3 diffuse = diff * lightColor; // Multiply diffuse light to light color (and even intensity)

	/* AMBIENT */
	vec3 ambientCol = ambientStr * ambientColor; // NOTE: ambientCol != ambientColor; ambientCol is the final ambient

	/* SPECULAR */
	vec3 viewDir = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal); // Reflection vector	
	float spec = pow(max(dot(reflectDir, viewDir), 0.1f), specPhong); // Specular light
	vec3 specCol = spec * specStr * lightColor; // Or any in light color; you can choose your own rgb as opposed to lightColor; final specular color

	/* POINT */
	// float distance = ;
	// float intensity = 1 / (distance * distance);

	FragColor = vec4(specCol + diffuse + ambientCol, 1.0f) * pixelColor; // Assign the pixels, given our UV, to the model of our object; The wrapping part
																// Apply the diffuse
																// Apply specular
}