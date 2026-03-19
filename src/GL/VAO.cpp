#include "VAO.h"

void VAO::Setup() {
	glGenVertexArrays(1, &ID);
}

void VAO::LinkVBO(VBO& VBO, GLuint layout, GLuint numComp, GLenum type, GLsizeiptr stride, void* offset) {
	VBO.Bind();
	glVertexAttribPointer(layout, numComp, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}

void VAO::Bind() {
	glBindVertexArray(ID);
}

void VAO::Unbind() {
	glBindVertexArray(0);
}

void VAO::Delete() {
	glDeleteVertexArrays(1, &ID);
}