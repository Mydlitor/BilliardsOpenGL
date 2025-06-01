#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 modelMatrix;
uniform mat4 camMatrix;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;

void main()
{
    FragPos = vec3(modelMatrix * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(modelMatrix))) * aNormal;
    TexCoord = aTexCoord;
    gl_Position = camMatrix * modelMatrix * vec4(aPos, 1.0);
}