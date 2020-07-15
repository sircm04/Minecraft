#version 330 core

layout (location = 0) out vec4 color;

in vec2 s_TexCoord;

uniform sampler2D image;
uniform vec4 u_color;

void main()
{
	color = texture(image, s_TexCoord) * u_color;

	if (color.a < 0.1)
		discard;
}