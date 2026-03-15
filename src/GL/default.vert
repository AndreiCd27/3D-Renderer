#version 330 core
layout (location = 0) in vec4 aPosLH;
layout (location = 1) in vec4 aColor;

uniform mat4 perspectiveMatrix;
uniform vec4 camPosLH;
uniform float camY;

out vec3 color;

void main()
{
	float relX = (aPosLH.y - camPosLH.y) + (aPosLH.x - camPosLH.x);
    float relZ = (aPosLH.w - camPosLH.w) + (aPosLH.z - camPosLH.z);
	gl_Position = perspectiveMatrix * vec4(relX, aColor.x - camY, relZ, 1.0);
	color = vec3(aColor.y, aColor.z, aColor.w);
};