#version 330 core

layout(location = 0) in vec3 a_Position;

out vec3 s_Position;

uniform mat4 projection, view;

void main()
{
    s_Position = a_Position;

    vec4 pos = projection * view * vec4(a_Position, 1.0);
    gl_Position = pos.xyww;
}