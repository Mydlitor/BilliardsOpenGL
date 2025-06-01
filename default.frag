#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

// Grayscale filter uniform
uniform bool enableGrayscale;

// Sta³y kolor (np. zielony)
const vec3 baseColor = vec3(0.2, 0.8, 0.2);

// Proste œwiat³o kierunkowe
const vec3 lightDir = normalize(vec3(-0.5, -1.0, -0.3));
const vec3 lightColor = vec3(1.0, 1.0, 1.0);

void main()
{
    float diff = max(dot(normalize(Normal), -lightDir), 0.0);
    vec3 diffuse = diff * lightColor * baseColor;
    
    // Apply grayscale filter if enabled
    if (enableGrayscale) {
        // Standard grayscale conversion using luminance weights
        float gray = dot(diffuse, vec3(0.299, 0.587, 0.114));
        diffuse = vec3(gray);
    }
    
    FragColor = vec4(diffuse, 1.0);
}