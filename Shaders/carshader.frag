#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture_diffuse1;

uniform vec3 lightColor;

void main()
{
    float ambientStrength = 1.0f;
    vec3 ambient = ambientStrength * lightColor;
    
    color = vec4(ambient, 1.0f) * vec4(texture(texture_diffuse1, TexCoords));
}
