#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stl_reader.h" // .stl geometry file reader by sreiter https://github.com/sreiter/stl_reader

#include "shaderClass.h"
#include "GeometryLoader.h"
#include "VBO.h"
#include "EBO.h"
#include "VAO.h"
#include "Camera.h"

//bool _Engine3D_Started = false;

class Engine3D {
private:

	// Initialize a scene and a camera
	Camera* UserCamera = nullptr;
	Scene* MainScene = nullptr;

	// GLFW window object
	GLFWwindow* window = nullptr;
	int windowHeight = 0;
	int windowWidth = 0;
	float windowAspectRatio = 0.0f;

	// Shader
	Shader* shaderProgram = nullptr;
	VAO* VAO_1 = nullptr;
	VBO* VBO_1 = nullptr;
	EBO* EBO_1 = nullptr;

	GLuint instanceVBO;

	// Stores projection matrices of meshes relative to the camera
	std::vector<glm::mat4> modelMatrices;

	// Appearance
	struct {
		float R = 0.2f;
		float G = 0.3f;
		float B = 0.5f; 
		float A = 1.0f;
	} backgroundColor;

	void registerCameraInput(float FOVdeg, float zNear, float zFar);

public:

	Engine3D() = default;
	~Engine3D();
	// SETERS
	void setScene();
	void setCamera(double posX, double posY, double posZ);

	void setCamera(double posX, double posY, double posZ, float yaw, float pitch);

	int setupGLFW(const int WINDOW_WIDTH, const int WINDOW_HEIGHT, const char* WINDOW_TITLE);

	void setupShaders();

	void setupInstanceVBO(const int cntOfObj);

	inline void setBackground(float R, float G, float B, float A) { backgroundColor = { R,G,B,A }; };

	// GETTERS
	Camera* getCamera();

	Scene* getScene();

	inline int windowShouldClose() { return glfwWindowShouldClose(window); };
	inline Shader* getShader() { return shaderProgram; };

	inline AVertex GetAVertex(double x, double y, double z, int r, int g, int b) {
		float xhigh = (float)x; float xlow = (float)(x - (double)xhigh);
		float zhigh = (float)z; float zlow = (float)(z - (double)zhigh);
		return { xlow, xhigh, zlow, zhigh, (float)y, (float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f };
	}
	/*
	inline AVertex GetAVertex(float x, float y, float z, int r, int g, int b) {
		return { x, 0.0f, z, 0.0f, y, (float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f };
	}
	*/

	const GLuint GetUniformLocation(const char* uniformName);

	// OTHERS

	void initGameFrame();

	void configureGameFrame(float FOVdeg, float zNear, float zFar);

	void EngineTerminate();

	void LoadSTLGeomFile(const char *filePath, int R, int G, int B, float scale);

	void DEBUG_showCameraVectors();
};