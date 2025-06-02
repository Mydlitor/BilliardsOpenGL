#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
// Grayscale filter uniform
uniform bool enableGrayscale;
// Rainbow light filter uniform
uniform bool enableRainbowLight;
uniform float time;

void main()
{    
    vec4 skyboxColor = texture(skybox, TexCoords);
    
    // RAINBOW LIGHT COLOR
    vec3 finalLightColor = vec3(1.0); // Default white light
    if (enableRainbowLight && !enableGrayscale) {
        finalLightColor = vec3(
            sin(time * 1.0) * 0.4 + 0.5,
            sin(time * 1.0 + 2.094) * 0.4 + 0.5,
            sin(time * 1.0 + 4.188) * 0.4 + 0.5
        );
    }
    
    // Apply rainbow light effect
    skyboxColor.rgb *= finalLightColor;
    
    // Apply grayscale filter if enabled (after rainbow effect)
    if (enableGrayscale) {
        // Standard grayscale conversion using luminance weights
        float gray = dot(skyboxColor.rgb, vec3(0.299, 0.587, 0.114));
        skyboxColor.rgb = vec3(gray);
    }
    
    FragColor = skyboxColor;
}