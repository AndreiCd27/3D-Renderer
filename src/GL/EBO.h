#pragma once

#include <glad/glad.h>

class EBO {
public:
	GLuint ID;
	EBO() = default;
	EBO(GLuint* indicies, GLsizeiptr size);

	void Bind();
	void Unbind();
	void Delete();
};