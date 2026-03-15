#include "Camera.h"
#include "GeometryLoader.h"

Camera_Vector3 Camera_Vector3::operator+(const Camera_Vector3& dr) {
	return { dr.x + this->x, dr.y + this->y, dr.z + this->z };
}
Camera_Vector3& Camera_Vector3::operator+=(const Camera_Vector3& dr) {
	this->x += dr.x; this->y += dr.y; this->z += dr.z;
	return *this;
}
Camera_Vector3 Camera_Vector3::operator*(const Camera_Vector3& dr) {
	return { dr.x * this->x, dr.y * this->y, dr.z * this->z };
}
Camera_Vector3 Camera_Vector3::operator*(const float& scalar) {
	return { scalar * this->x, scalar * this->y, scalar * this->z };
}
Camera_Vector3 Camera_Vector3::operator^(const Camera_Vector3& dr) { //CROSS PRODUCT
	return {
		this->y * dr.z - this->z * dr.y,
		this->z * dr.x - this->x * dr.z,
		this->x * dr.y - this->y * dr.x
	};
}
Camera_Vector3 Camera_Vector3::Normalize() {
	float dist = std::sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);
	return { this->x / dist, this->y / dist, this->z / dist };
}

Camera::Camera(Camera_Vector3 pos, float _Yaw, float _Pitch)
{
	Position = pos;
	Yaw = _Yaw;
	Pitch = _Pitch;
}

const GLfloat scalingFactor = 0.001f;

void Camera::Matrix(float FOVdeg, float near, float far, float aspect, Shader& shader) {
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	glm::vec3 camDir = glm::normalize(front);

	glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);

	mat4Tuple.view = glm::lookAt(glm::vec3(0.0f), camDir, camUp);
	mat4Tuple.proj = glm::perspective(glm::radians(FOVdeg), aspect, near, far);

	glm::mat4 perspMatrix = mat4Tuple.proj * mat4Tuple.view;

	float xhigh = (float)this->Position.x; float xlow = (float)(this->Position.x - (double)xhigh);
	float zhigh = (float)this->Position.z; float zlow = (float)(this->Position.z - (double)zhigh);

	glUniform1f(this->camYLoc, this->Position.y);
	glUniform4f(this->camPosUniformLoc, xlow, xhigh, zlow, zhigh);
	glUniformMatrix4fv(this->perspMat4Loc, 1, GL_FALSE, glm::value_ptr(perspMatrix));
}

void Camera::Inputs(GLFWwindow* window)
{

	// Handles key inputs

	// ROTATION (Q, E, R, T)
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) Yaw -= sensitivity;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) Yaw += sensitivity;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) Pitch -= sensitivity;
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) Pitch += sensitivity;

	if (Pitch > 89.0f) Pitch = 89.0f;
	if (Pitch < -89.0f) Pitch = -89.0f;

	// Recalculate Rotation Vector
	Camera_Vector3 newRotation;
	newRotation.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	newRotation.y = sin(glm::radians(Pitch));
	newRotation.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

	this->Rotation = newRotation;

	// (W, A, S, D, I, O)
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) Position += Rotation * speed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) Position += Rotation * -speed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) Position += (Rotation ^ Up).Normalize() * -speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) Position += (Rotation ^ Up).Normalize() * speed;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) Position += Up * -speed;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) Position += Up * speed;

	// Avoid camera rolling
	if (Pitch > 89.0f) Pitch = 89.0f;
	if (Pitch < -89.0f) Pitch = -89.0f;
}