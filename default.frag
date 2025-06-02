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
    
    // Improved lighting with ambient component
    vec3 normal = normalize(Normal);
    float diff = max(dot(normal, -lightDir), 0.0);
    
    // Add ambient light to prevent complete darkness
    vec3 ambient = 0.3 * lightColor * color;
    vec3 diffuse = diff * lightColor * color;
    vec3 finalColor = ambient + diffuse;
    
    // Apply grayscale filter if enabled
    if (enableGrayscale) {
        // Standard grayscale conversion using luminance weights
        float gray = dot(finalColor, vec3(0.299, 0.587, 0.114));
        finalColor = vec3(gray);
    }
    
    FragColor = vec4(finalColor, 1.0);
}