#pragma once

#include <glad/glad.h>
#include <iostream>

class EBO {
public:
	GLuint ID;
	size_t Capacity = 0;
	EBO() { std::cout << "C -> EBO \n"; };

	void Setup(GLuint* indicies, GLsizeiptr size, const int drawStyle);
	void Bind();
	void Unbind();
	void Delete();
};