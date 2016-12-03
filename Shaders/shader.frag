#version 330 core
in vec3 Normal;
in vec3 FragPos;

in float safeland;
in float IsSafeLaneAfterSafeLane;
in float IsNormalLaneBeforeNormalLane;
in float xposition;
out vec4 color;

uniform vec3 lightColor;

void main() {
    
    //Ambient strength * light Color to get the final light falling on the object
    float ambientStrength = 1.0f;
    vec3 ambient = ambientStrength * lightColor;
    
    vec3 originalColor = vec3(0.28627f, 0.3098f, 0.37254f);
    if (safeland == 1.0f && IsSafeLaneAfterSafeLane == 0.0f){
        originalColor =vec3(0.635f, 0.827450f, 0.34901f);
    }
    if(safeland == 1.0f && IsSafeLaneAfterSafeLane == 1.0f){
         originalColor = vec3(0.6588f, 0.8549f, 0.380f);
    }
    if(safeland == 1.0f && Normal == vec3(0.0f, 0.0f, 1.0f)&& IsSafeLaneAfterSafeLane == 0.0f){
        originalColor = vec3(0.46666f, 0.6156f, 0.2745f);
    }
    if(IsNormalLaneBeforeNormalLane == 1.0f &&
       (
        xposition > 9 && xposition < 12 ||
        xposition < 6 && xposition > 3 ||
        xposition < 0 && xposition > -3 ||
        xposition < -6 && xposition > -9
        )){
        originalColor = vec3(0.443137f, 0.47843f, 0.5725f);
    }
    
    vec3 result = (ambient) * originalColor;
    color = vec4(result, 1.0f);
}
