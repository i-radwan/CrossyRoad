#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out float safeland;
out float IsSafeLaneAfterSafeLane;
out float IsNormalLaneBeforeNormalLane;
out vec3 Normal;
out vec3 FragPos;
out float xposition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


uniform float isSafeLane = 0.0;
uniform float isSafeLaneAfterSafeLane = 0.0;
uniform float isNormalLaneBeforeNormalLane = 0.0;
void main() {
    gl_Position = projection * view * model * vec4(position.x, position.y, position.z, 1.0f);
    Normal = mat3(transpose(inverse(model))) * normal;
    FragPos = vec3(model * vec4(position, 1.0f));
    
    IsSafeLaneAfterSafeLane=isSafeLaneAfterSafeLane;
    IsNormalLaneBeforeNormalLane=isNormalLaneBeforeNormalLane;
    safeland = isSafeLane;
    xposition = position.x;
}
