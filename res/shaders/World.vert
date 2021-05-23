#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_TexCoord;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in float a_AO;

out vec3 s_Position;
out vec3 s_TexCoord;
out vec3 s_Normal;
out float s_AO;
flat out float s_Occlusion;

uniform mat4 projection, view;

void main()
{
	s_Position = a_Position;
	s_TexCoord = a_TexCoord;
	s_Normal = a_Normal;
	s_AO = a_AO;

	s_Occlusion = 4;
	
	if (s_Normal == vec3(0.0, 0.0, 1.0) || s_Normal == vec3(0.0, 0.0, -1.0))
		s_Occlusion = 3;
	if (s_Normal == vec3(1.0, 0.0, 0.0) || s_Normal == vec3(-1.0, 0.0, 0.0))
		s_Occlusion = 2;
	if (s_Normal == vec3(0.0, -1.0, 0.0))
		s_Occlusion = 1;

    gl_Position = projection * view * vec4(s_Position, 1.0);
}
