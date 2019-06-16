#version 330 core

struct Light {
    vec3 lightPos;
    vec3 lightColor;
};

// Output from vertex shader input to the fragment shader.
in vec2 texCoords;
in vec3 Normal;
in vec3 FragPos;

//// Output from the vertex shader.
out vec4 FragColor;

uniform sampler2D image;
uniform vec3 viewDir;
uniform Light lights[4];

//function prototypes //
vec3 calcPointLights(Light lights, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 Color);

//main function start
void main()
{
    vec3 Color = texture(image, texCoords).rgb; 
    vec3 normal = normalize(Normal);
    
    vec3 ambient = 0.0f * Color;
    vec3 viewDir = normalize(viewDir - FragPos);
    vec3 lighting = vec3(0.0f);
    
    for (int i = 0; i < 4; i++) {
    
        lighting += calcPointLights(lights[i], normal, FragPos, viewDir, Color);
    }
    
    vec3 result = ambient + lighting;
    
    FragColor = vec4(result, 1.0f);
}       //end main

    //function to calculate the point lights ////
vec3 calcPointLights(Light lights, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 Color)
{
    vec3 lightDir = normalize(lights.lightPos - FragPos);
    
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lights.lightColor * diff * Color;
    
    float distance = length(fragPos - lights.lightPos);
    diffuse *= 1.0 / (distance * distance);
    
    return diffuse;
}           //calcPointLights.
