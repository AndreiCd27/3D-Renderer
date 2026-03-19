#version 330 core
layout (location = 0) in vec4 aPosLH;
layout (location = 1) in vec4 aColor;

uniform mat4 lightPerspMatrix;

out vec3 color;
out vec4 vertexPosition;
out vec4 vertexShadowPosition;

void main()
{
	float relX = aPosLH.y + aPosLH.x;
    float relZ = aPosLH.w + aPosLH.z;
	gl_Position = lightPerspMatrix * vec4(relX, aColor.x, relZ, 1.0);
	color = vec3(aColor.y, aColor.z, aColor.w);
};