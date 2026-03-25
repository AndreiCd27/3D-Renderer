#pragma once

#include "pch.h"
#include "framework.h"

std::string get_file_contents(const char* filename);

class Shader {
public:
	GLuint ID;
	Shader() = default;
	void Setup(const char* vertFileName, const char* fragFileName);
	void Activate();
	void Delete();
	// Checks if the different Shaders have compiled properly
	void compileErrors(unsigned int shader, const char* type);
};