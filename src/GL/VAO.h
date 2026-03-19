#pragma once

#include <glad/glad.h>
#include "VBO.h"

class VAO {
public:
	GLuint ID;
	VAO() { std::cout << "C -> VAO \n"; };

	void Setup();
	void LinkVBO(VBO& VBO, GLuint layout, GLuint numComp, GLenum type, GLsizeiptr stride, void* offset);
	void Bind();
	void Unbind();
	void Delete();
};