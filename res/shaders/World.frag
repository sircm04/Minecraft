#version 330 core

layout(location = 0) out vec4 color;

in vec3 s_Position;
in vec3 s_TexCoord;
in vec3 s_Normal;
in float s_AO;
flat in float s_Occlusion;

uniform sampler2DArray image;

void main()
{
	color = texture(image, s_TexCoord) * vec4(vec3((s_Occlusion + (s_AO + 1)) * 0.125), 1.0);
}
