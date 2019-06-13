#version 330 core
layout (location = 0) aPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec3(aPosition, 1.0f);
}
