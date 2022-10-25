/* POINT LIGHT */

#version 330 core //shader version

uniform sampler2D tex0;

uniform vec3 lightPos; // light source
uniform vec3 lightColor; // color of the light source

uniform float ambientStr; // ambient intensity or strength
uniform vec3 ambientColor; // color of the ambient or reflected light

uniform vec3 cameraPos;
uniform float specStr; // specular intensity or strength
uniform float specPhong; // spread or concentration of the specular light

in vec2 texCoord; // Receive the output variable for textures in sample.vert
in vec3 normCoord;
in vec3 fragPos;

out vec4 FragColor;


void main(){
    vec3 normal = normalize(normCoord);
    vec3 lightDir = normalize(lightPos - fragPos); // Direction from the light source to your fragment source

    /* DIFFUSE */
    float diff = max(dot(normal, lightDir), 0.0f); // Max so we do not have any negative lights
    vec3 diffuse = diff * lightColor; // Multiply diffuse light to light color (and even intensity)

    /* AMBIENT */
    vec3 ambientCol = ambientStr * ambientColor; // NOTE: ambientCol != ambientColor; ambientCol is the final ambient

    /* SPECULAR */
    vec3 viewDir = normalize(cameraPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal); // Reflection vector    
    float spec = pow(max(dot(reflectDir, viewDir), 0.1f), specPhong); // Specular light
    vec3 specCol = spec * specStr * lightColor; // Or any in light color; you can choose your own rgb as opposed to lightColor; final specular color

    /* POINT */
    float distance = length(lightPos - fragPos); // solve for the distance between the position of the light source and the object source
    float attenuation = 1.0f / (distance * distance); // solve for the attenuation via. inverse law; (1.0 / distance ^ 2)

    /* APPLY POINT LIGHT INTENSITY */
    diffuse = diffuse * attenuation;        // transform the diffuse vector with respect to the attenuation
    ambientCol = ambientCol * attenuation;    // transform the ambient vector with respect to the attenuation
    specCol = specCol * attenuation;        // transform the specular vector with respect to the attenuation

    FragColor = vec4(specCol + diffuse + ambientCol, 1.0f) * texture(tex0, texCoord); // Assign the pixels, given our UV, to the model of our object; The wrapping part
                                                                // Apply the diffuse
                                                                // Apply the ambient
                                                                // Apply specular
}