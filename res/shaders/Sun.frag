#version 330 core

layout(location = 0) out vec4 color;

in vec2 s_TexCoord;

uniform sampler2D image;

void main()
{
    color = texture(image, s_TexCoord);
}