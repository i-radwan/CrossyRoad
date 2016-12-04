#version 330 core
in vec3 Normal;
in vec3 FragPos;

out vec4 color;

uniform vec4 diffuseColor;
uniform vec4 ambientColor;
uniform vec4 specularColor;
uniform vec4 emiColor;

uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
    //Ambient strength * light Color to get the final light falling on the object
    float ambientStrength = 1.0f;
    vec3 ambient = ambientStrength * lightColor;
    
    
    //Diffuse Lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    
    vec4 result = vec4(ambient + diffuse, 1.0) * diffuseColor;
    color = vec4(result);
}
