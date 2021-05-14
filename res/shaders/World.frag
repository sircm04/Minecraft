#version 330 core

layout(location = 0) out vec4 color;

in vec3 s_Position;
in vec3 s_TexCoord;
in vec3 s_Normal;
flat in uint s_AO;

uniform sampler2DArray image;

void main()
{
	color = texture(image, s_TexCoord) * vec4(vec3(int(s_AO + 1u) * 0.25), 1.0);
}
