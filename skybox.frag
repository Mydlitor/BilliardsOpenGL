#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
// Grayscale filter uniform
uniform bool enableGrayscale;

void main()
{    
    vec4 skyboxColor = texture(skybox, TexCoords);
    
    // Apply grayscale filter if enabled
    if (enableGrayscale) {
        // Standard grayscale conversion using luminance weights
        float gray = dot(skyboxColor.rgb, vec3(0.299, 0.587, 0.114));
        skyboxColor.rgb = vec3(gray);
    }
    
    FragColor = skyboxColor;
}