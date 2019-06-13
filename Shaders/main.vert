#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

/// uniform variables model matrix, view matrix and projection matrix //
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// output variables
out vec2 texCoords;
out vec3 Normal;
out vec3 FragPos;

//main function
void main()
{
    gl_Position = projection * view * model * vec4(aPosition, 1.0f);
    FragPos = vec3(model * vec4(aPosition, 1.0f));
    texCoords = aTexCoords;
    Normal = mat3(transpose(inverse(model))) * aNormal;
}    //end main
