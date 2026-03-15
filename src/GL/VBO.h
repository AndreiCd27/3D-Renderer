#pragma once

#include <glad/glad.h>

#include "GeometryLoader.h"

class VBO {
public:
	GLuint ID;
	VBO() = default;
	VBO(AVertex* vertices, GLsizeiptr size);
	VBO(int* vertices, GLsizeiptr size);

	void Bind();
	void Unbind();
	void Delete();
};