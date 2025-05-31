#ifndef SKYBOX_CLASS_H
#define SKYBOX_CLASS_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include "shaderClass.h"
#include "Camera.h"

class Skybox
{
public:
    GLuint textureID;
    GLuint VAO, VBO;
    Shader* skyboxShader;

    Skybox(const std::vector<std::string>& faces);

    ~Skybox();

    void Draw(Camera& camera, int width, int height);

    void Delete();

private:
    GLuint loadCubemap(const std::vector<std::string>& faces);

    void setupSkybox();
};

#endif