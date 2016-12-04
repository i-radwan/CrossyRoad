#version 330 core

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform sampler2D texture_diffuse1;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform float factor;

void main()
{
    float ambientStrength = 1.0f;
    vec3 ambient = ambientStrength * lightColor;
    
    
    //Diffuse Lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff*factor * lightColor;
    
    color = vec4(ambient + diffuse , 1.0f) * vec4(texture(texture_diffuse1, TexCoords));
}
