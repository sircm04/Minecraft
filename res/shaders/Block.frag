#version 330 core

layout(location = 0) out vec4 color;

in vec3 s_Position;
in vec3 s_TexCoord;
in vec3 s_Normal;
in float s_AO;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform sampler2DArray image;

uniform vec3 playerPosition;
uniform vec2 fogDist;
uniform vec3 fogColor;

void main()
{
//  const vec3 sunDir = normalize(vec3(1, 3, 2));
//	const float ambient = 0.4;
//	
//	float diffuse = max(dot(s_Normal, sunDir), 0.0);
	color = /*vec4(vec3(ambient + diffuse), 1.0) * */texture(image, s_TexCoord);
    float fogFactor = clamp((fogDist.y - distance(playerPosition, s_Position)) / (fogDist.y - fogDist.x), 0.0, 1.0);

	color *= vec4(vec3((s_AO + 1) * 0.25), 1.0);
    color = mix(vec4(fogColor, 1.0), color, fogFactor);
}