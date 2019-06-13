#version 330 core

out vec4 color;
in vec2 TexCoords;

uniform float exposure;
uniform sampler2D hdrBuffer;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
    
    //Reinhard Tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    
    //Gamma Correction
    mapped = pow(mapped, vec3(1.0 / gamma));
    
    color = vec4(hdrColor, 1.0);
}
