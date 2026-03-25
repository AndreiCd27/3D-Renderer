#pragma once

#include "pch.h"
#include "framework.h"

class EBO {
public:
	GLuint ID;
	size_t Capacity = 0;
	EBO() { std::cout << "C -> EBO \n"; };

	void Setup(std::vector<GLuint>& VertIndicies, GLsizeiptr size, const int drawStyle);
	void Bind();
	void Unbind();
	void Delete();
};