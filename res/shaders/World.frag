#version 330 core

layout(location = 0) out vec4 color;

in vec3 s_Position;
in vec3 s_TexCoord;
in vec3 s_Normal;

void main()
{
	color = vec4(1.0, 1.0, 1.0, 1.0);
}