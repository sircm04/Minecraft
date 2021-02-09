#version 330 core

layout(location = 0) out vec4 color;

in vec3 s_Position;
in vec3 s_TexCoord;
in vec3 s_Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform sampler2DArray image;

uniform vec3 playerPosition;
uniform vec2 fogDist;
uniform vec3 fogColor;

void main()
{
    // ambient
    float ambientStrength = 0.8;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse
    vec3 norm = normalize(s_Normal);
    vec3 lightDir = normalize(vec3(0, 1, 0));
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    //float specularStrength = 0.5;
    //vec3 viewDir = normalize(viewPos - s_Position);
    //vec3 reflectDir = reflect(-lightDir, norm);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);
    //vec3 specular = specularStrength * spec * lightColor;
    
    vec4 result = vec4(max(ambient, diffuse)/* + specular*/, 1.0) * texture(image, s_TexCoord);
    float fogFactor = clamp((fogDist.y - distance(playerPosition, s_Position)) / (fogDist.y - fogDist.x), 0.0, 1.0);

    color = mix(vec4(fogColor, 1.0), result, fogFactor);
}