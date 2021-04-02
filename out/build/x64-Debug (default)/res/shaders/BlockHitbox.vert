#version 330 core

layout(location = 0) in vec3 a_Position;

uniform mat4 projection, view, model;

out vec3 s_Position;

void main()
{
    s_Position = a_Position;

    gl_Position = projection * view * model * vec4(a_Position, 1.0);
}