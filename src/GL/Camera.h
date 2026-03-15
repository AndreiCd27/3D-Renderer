#pragma once

#include "shaderClass.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Could have used glm
class Camera_Vector3 {
public:
	double x, y, z;
	Camera_Vector3 operator+(const Camera_Vector3& dr);
	Camera_Vector3 operator*(const Camera_Vector3& dr);
	Camera_Vector3 operator*(const float& scalar);
	Camera_Vector3& operator+=(const Camera_Vector3& dr);
	Camera_Vector3 operator^(const Camera_Vector3& dr); // Used for cross product
	Camera_Vector3 Normalize();
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
	Camera_Vector3 Position;
	Camera_Vector3 Rotation = {0.0f, 0.0f, -1.0f};
	Camera_Vector3 Up = { 0.0f, 1.0f, 0.0f };
	float Yaw, Pitch;
	Mat4Pair mat4Tuple;

	float speed = 0.1f;
	float sensitivity = 0.01f;

	GLuint camPosUniformLoc;
	GLuint perspMat4Loc;
	GLuint camYLoc;

	// Camera constructor to set up initial values
	Camera(Camera_Vector3 pos, float _Yaw, float _Pitch);

	// Sends the camera perspective matrix to the Vertex Shader
	void Matrix(float FOVdeg, float nearPlane, float farPlane, float aspectRatio, Shader& shader);
	// Handles camera inputs
	void Inputs(GLFWwindow* window);
};