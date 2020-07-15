#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_TexCoord;
layout(location = 2) in vec3 a_Normal;

out vec3 s_Position;
out vec3 s_TexCoord;
out vec3 s_Normal;

uniform mat4 projection, view, model;

void main()
{
	s_Position = vec3(model * vec4(a_Position, 1.0));
	s_TexCoord = a_TexCoord;
	s_Normal = mat3(transpose(inverse(model))) * a_Normal;

    gl_Position = projection * view * vec4(s_Position, 1.0);
}