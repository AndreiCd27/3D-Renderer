#pragma once

#include "shaderClass.h"

class Texture {
public:
	GLuint FBO_ID;
	GLuint depthTexture;

	void setupFBO();
	bool setupDepthTexture(const int SIZE, const unsigned int shadowMapLocation);
};