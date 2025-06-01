#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 FragColor;

// Grayscale filter uniform
uniform bool enableGrayscale;
uniform sampler2D texture_diffuse1;
uniform int hasTexture;
uniform vec4 baseColor;

const vec3 lightDir = normalize(vec3(-0.5, -1.0, -0.3));
const vec3 lightColor = vec3(1.0, 1.0, 1.0);

void main()
{
    vec3 color = hasTexture == 1 ? texture(texture_diffuse1, TexCoord).rgb : baseColor.rgb;
    float diff = max(dot(normalize(Normal), -lightDir), 0.0);
    vec3 diffuse = diff * lightColor * color;
    
    // Apply grayscale filter if enabled
    if (enableGrayscale) {
        // Standard grayscale conversion using luminance weights
        float gray = dot(diffuse, vec3(0.299, 0.587, 0.114));
        diffuse = vec3(gray);
    }
    
    FragColor = vec4(diffuse, 1.0);
}