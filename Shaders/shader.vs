#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out float safeland;
out float IsSafeLaneAfterSafeLane;
out float IsNormalLaneBeforeNormalLane;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
} vs_out;


out float xposition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

uniform float isSafeLane = 0.0;
uniform float isSafeLaneAfterSafeLane = 0.0;
uniform float isNormalLaneBeforeNormalLane = 0.0;
void main() {
    gl_Position = projection * view * model * vec4(position.x, (position.y), position.z, 1.0f);
    vs_out.Normal = transpose(inverse(mat3(model))) * normal;
    vs_out.FragPos = vec3(model * vec4(position, 1.0));
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    
    IsSafeLaneAfterSafeLane=isSafeLaneAfterSafeLane;
    IsNormalLaneBeforeNormalLane=isNormalLaneBeforeNormalLane;
    safeland = isSafeLane;
    xposition = position.x;
}
