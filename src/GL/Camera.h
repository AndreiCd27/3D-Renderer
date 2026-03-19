#pragma once

#include "shaderClass.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Could have used glm
class AVector3 {
public:
	double x = 0.0f, y = 0.0f, z = 0.0f;
	AVector3() = default;
	AVector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {};
	~AVector3() = default;
	AVector3 operator+(const AVector3& dr);
	AVector3 operator*(const AVector3& dr);
	AVector3 operator*(const float& scalar);
	AVector3& operator+=(const AVector3& dr);
	AVector3 operator^(const AVector3& dr); // Used for cross product
	AVector3 Normalize();
};

// Here we store our view and projection matrix and we forward it to Main.cpp
// Such that we only store the resulting matrix for each instance,
// Without looking for uniforms for our shaders every frame
struct Mat4Pair {
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);
};

class Camera
{
public:
	// Stores the main vectors of the camera
	AVector3 Position;
	AVector3 Rotation = {0.0f, 0.0f, -1.0f};
	AVector3 Up = { 0.0f, 1.0f, 0.0f };
	float Yaw, Pitch;
	Mat4Pair mat4Tuple;

	float speed = 0.1f;
	float sensitivity = 0.05f;

	GLuint camPosUniformLoc;
	GLuint perspMat4Loc;
	GLuint userPerspMat4Loc; //for first pass when drawing from SunCamera perspective in Texture
	GLuint camYLoc;

	// Camera constructor to set up initial values
	Camera(AVector3 pos, float _Yaw, float _Pitch);

	// Sends the camera perspective matrix to the Vertex Shader
	void Matrix(float FOVdeg, float nearPlane, float farPlane, float aspectRatio, Shader& shader);

	void LightMatrix(float shadowMapScale, Shader& shader, GLuint depthMatrixID, GLuint perspMatrixIDUser, bool TextureBias);
	// Handles camera inputs
	void Inputs(GLFWwindow* window);
};