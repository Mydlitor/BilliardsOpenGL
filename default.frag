#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 FragColor;

uniform bool enableGrayscale;
uniform bool enableRainbowLight; // <-- NOWE
uniform float time;              // <-- NOWE

uniform vec3 camPos;
uniform vec4 baseColor;
uniform vec3 lightPos;
uniform vec3 lightColor;

uniform sampler2D texture_diffuse1;
uniform int hasTexture;

void main()
{
    vec3 color = hasTexture == 1 ? texture(texture_diffuse1, TexCoord).rgb : baseColor.rgb;

    // AMBIENT
    float ambient = 0.2f;

    // DIFFUSE
    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(lightPos - FragPos);
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    // SPECULAR
    float specularStrength = 1.0f;
    vec3 viewDirection = normalize(camPos - FragPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
    float specular = spec * specularStrength;

    // RAINBOW LIGHT COLOR
    vec3 finalLightColor = lightColor;
    if (enableRainbowLight && !enableGrayscale) {
        finalLightColor = vec3(
            sin(time * 1.0) * 0.4 + 0.5,
            sin(time * 1.0 + 2.094) * 0.4 + 0.5,
            sin(time * 1.0 + 4.188) * 0.4 + 0.5
        );
    }

    float lighting = ambient + diffuse + specular;
    vec3 result = color * finalLightColor * lighting;

    if (enableGrayscale) {
        float gray = dot(result, vec3(0.299, 0.587, 0.114));
        result = vec3(gray);
    }

    FragColor = vec4(result, 1.0);
}
