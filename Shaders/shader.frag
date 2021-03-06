#version 330 core
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D shadowMap;

in float safeland;
in float IsSafeLaneAfterSafeLane;
in float IsNormalLaneBeforeNormalLane;
in float xposition;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool shadows;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - (fs_in.FragPos));
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // Check whether current frag pos is in shadow
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}

void main() {
    vec3 originalColor = vec3(0.28627f, 0.3098f, 0.37254f);
    if (safeland == 1.0f && IsSafeLaneAfterSafeLane == 0.0f){
        originalColor =vec3(0.635f, 0.827450f, 0.34901f);
    }
    if(safeland == 1.0f && IsSafeLaneAfterSafeLane == 1.0f){
        originalColor = vec3(0.6588f, 0.8549f, 0.380f);
    }
    if(safeland == 1.0f && fs_in.Normal == vec3(0.0f, 0.0f, 1.0f)&& IsSafeLaneAfterSafeLane == 0.0f){
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
    
    
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.7f);
    
    // Ambient
    vec3 ambient = 0.4 * originalColor;
    // Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // Calculate shadow
    float shadow = shadows ? ShadowCalculation(fs_in.FragPosLightSpace) : 0.0;
    
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse)) * originalColor;
    FragColor = vec4(lighting, 1.0f);
}
