#pragma once

#include <glad/glad.h>

#include "GeometryLoader.h"

class VBO {
public:
	GLuint ID;
	size_t Capacity = 0;
	VBO() { std::cout << "C -> VBO \n"; };

	void Setup(AVertex* vertices, GLsizeiptr size, const int drawStyle);
	void Setup(int* vertices, GLsizeiptr size, const int drawStyle);
	void Bind();
	void Unbind();
	void Delete();
};