#version 330 core

out vec4 color;

uniform vec4 diffuseColor;
uniform vec4 ambientColor;
uniform vec4 specularColor;
uniform vec4 emiColor;

void main()
{
    color = diffuseColor;
}
