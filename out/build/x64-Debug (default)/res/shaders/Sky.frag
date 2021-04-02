#version 330 core

layout(location = 0) out vec4 color;

in vec3 s_Position;

void main()
{
    color = mix(vec4(0.67, 0.82, 1.0, 1.0), vec4(0.49, 0.67, 1.0, 1.0), smoothstep(-0.01, 0.01, s_Position.y));
}