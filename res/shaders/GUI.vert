#version 330 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 s_TexCoord;

uniform mat4 projection, model;

void main()
{
	s_TexCoord = a_TexCoord;

	gl_Position = projection * model * vec4(a_Position, 0.0, 1.0);
}