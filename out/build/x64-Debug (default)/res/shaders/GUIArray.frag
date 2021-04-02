#version 330 core

layout (location = 0) out vec4 color;

in vec2 s_TexCoord;

uniform sampler2DArray image;
uniform vec4 u_color;

uniform float texZ;

void main()
{
	color = texture(image, vec3(s_TexCoord, texZ)) * u_color;
}