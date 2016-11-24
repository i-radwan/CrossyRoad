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
    float zPos =0;
    if(isNormalLaneBeforeNormalLane == 1.0f){
        zPos = position.z;
    }else{
        zPos = ((isSafeLane == 1.0f) ? position.z * 0.8: position.z * 2);
    }
    gl_Position = projection * view * model * vec4(position.x * 25, ((isSafeLane == 1.0f) ? position.y+0.2f:position.y), zPos, 1.0f);
    Normal = mat3(transpose(inverse(model))) * normal;
    FragPos = vec3(model * vec4(position, 1.0f));
    
    IsSafeLaneAfterSafeLane=isSafeLaneAfterSafeLane;
    IsNormalLaneBeforeNormalLane=isNormalLaneBeforeNormalLane;
    safeland = isSafeLane;
    xposition = position.x * 25;
}
