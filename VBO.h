#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glad/glad.h>

class VBO
{
public:
	// Reference ID of the Vertex Buffer Object
	GLuint ID;
	GLsizeiptr size;
	
	// Domyœlny konstruktor
	VBO() : ID(0), size(0) {}
	
	// Constructor that generates a Vertex Buffer Object and links it to vertices
	VBO(GLfloat* vertices, GLsizeiptr size);

	// Binds the VBO
	void Bind();
	// Unbinds the VBO
	void Unbind();
	// Deletes the VBO
	void Delete();
	// Zwraca rozmiar bufora
	GLsizeiptr GetSize() const { return size; }
};

#endif
