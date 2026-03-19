#pragma once

#include <glad/glad.h>
#include <iostream>

class EBO {
public:
	GLuint ID;
	EBO() { std::cout << "C -> EBO \n"; };

	void Setup(GLuint* indicies, GLsizeiptr size, const int drawStyle);
	void Bind();
	void Unbind();
	void Delete();
};