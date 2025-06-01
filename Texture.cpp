#include"Texture.h"
#include <iostream>

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
	type = texType;
	int widthImg, heightImg, numColCh;	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 4);
	if (!bytes) {
		std::cerr << "[Texture DIAG] Failed to load texture: " << image << std::endl;
	} else {
		std::cout << "[Texture DIAG] Loaded texture: " << image
				  << " (" << widthImg << "x" << heightImg << ", channels: " << numColCh << ")" << std::endl;
	}

	glGenTextures(1, &ID);
	glActiveTexture(slot);
	glBindTexture(texType, ID);
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);

	GLenum err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cerr << "[Texture DIAG] OpenGL error after glTexImage2D: 0x" << std::hex << err << std::endl;
	}

	glGenerateMipmap(texType);	stbi_image_free(bytes);
	glBindTexture(texType, 0);

	if (ID == 0) {
		std::cerr << "[Texture DIAG] Invalid texture ID!" << std::endl;
	}
}

Texture::Texture(const unsigned char* data, int dataSize, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
	type = texType;
	int widthImg, heightImg, numColCh;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load_from_memory(data, dataSize, &widthImg, &heightImg, &numColCh, 0);
	std::cout << "[Texture DIAG] Buffer size: " << dataSize << std::endl;
	if (!bytes) {
		std::cerr << "[Texture DIAG] Failed to load texture from memory!" << std::endl;
	} else {
		std::cout << "[Texture DIAG] Loaded texture from memory (" << widthImg << "x" << heightImg << ", channels: " << numColCh << ")" << std::endl;
	}

	glGenTextures(1, &ID);
	glActiveTexture(slot);
	glBindTexture(texType, ID);
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);

	glGenerateMipmap(texType);
	stbi_image_free(bytes);
	glBindTexture(texType, 0);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	shader.Activate();
	glUniform1i(texUni, unit);
}
void Texture::Bind()
{
	glActiveTexture(GL_TEXTURE0);	glBindTexture(type, ID);
	if (ID == 0) {
		std::cerr << "[Texture DIAG] Trying to bind invalid texture!" << std::endl;
	}
}
void Texture::Unbind()
{
	glBindTexture(type, 0);
}
void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}