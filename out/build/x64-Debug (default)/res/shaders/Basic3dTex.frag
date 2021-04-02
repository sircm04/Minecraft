#version 330 core

layout(location = 0) out vec4 color;

in vec3 s_TexCoord;

uniform sampler2DArray image;

void main()
{
    color = texture(image, s_TexCoord);
}