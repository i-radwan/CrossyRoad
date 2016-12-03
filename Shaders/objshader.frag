#version 330 core

out vec4 color;

uniform vec4 diffuseColor;
uniform vec4 ambientColor;
uniform vec4 specularColor;
uniform vec4 emiColor;

uniform vec3 lightColor;

void main()
{
    //Ambient strength * light Color to get the final light falling on the object
    float ambientStrength = 1.0f;
    vec3 ambient = ambientStrength * lightColor;
    
    vec4 result = vec4(ambient, 1.0) * diffuseColor;
    color = vec4(result);
}
