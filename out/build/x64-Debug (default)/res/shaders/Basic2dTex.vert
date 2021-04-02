#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 s_TexCoord;

uniform mat4 projection, view, model;

void main()
{
	s_TexCoord = a_TexCoord;

    gl_Position = projection * view * model * vec4(a_Position, 1.0);
}