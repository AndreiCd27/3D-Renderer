#version 330 core
layout (location = 0) in vec4 aPosLH;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec4 aNormal; //only first 3 positions, 4th is MESH_ID (can be used later for UV coord)

uniform mat4 perspectiveMatrix;
uniform vec4 camPosLH;
uniform float camY;

uniform mat4 lightPerspMatrix;

out vec3 color;
out vec3 normalVector;
out vec4 vertexShadowPosition;

void main()
{
	float relX = (aPosLH.y - camPosLH.y) + (aPosLH.x - camPosLH.x);
    float relZ = (aPosLH.w - camPosLH.w) + (aPosLH.z - camPosLH.z);
	gl_Position = perspectiveMatrix * vec4(relX, aColor.x - camY, relZ, 1.0);

	float worldX = aPosLH.y + aPosLH.x;
	float worldZ = aPosLH.w + aPosLH.z;

	vertexShadowPosition = lightPerspMatrix * vec4(worldX, aColor.x, worldZ, 1.0);

	color = vec3(aColor.y, aColor.z, aColor.w);

	normalVector = vec3(aNormal.x, aNormal.y, aNormal.z);
};