#pragma once
#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H
#include<glad/glad.h>

extern "C" {
    unsigned char *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels);
    void stbi_image_free(void *retval_from_stbi_load);
    int stbi_set_flip_vertically_on_load(int flag_true_if_should_flip);
}

#include"shaderClass.h"
class Texture
{
public:
	GLuint ID;
	GLenum type;
	Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum
		pixelType);
	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();
};
#endif
