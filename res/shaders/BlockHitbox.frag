#version 330 core

layout(location = 0) out vec4 color;

in vec3 s_Position;

void main()
{
	color = vec4(0.0, 0.0, 0.0, 0.5);
}