#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture_diffuse1; // Tekstura

const vec3 lightDir = normalize(vec3(-0.5, -1.0, -0.3));
const vec3 lightColor = vec3(1.0, 1.0, 1.0);

void main()
{
    vec3 baseColor = texture(texture_diffuse1, TexCoord).rgb;
    float diff = max(dot(normalize(Normal), -lightDir), 0.0);
    vec3 diffuse = diff * lightColor * baseColor;
    FragColor = vec4(diffuse, 1.0);
}