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
#include "Texture.h"

//bool _Engine3D_Started = false;

class Engine3D {
private:

	// Initialize a scene and a camera
	Camera UserCamera = Camera(AVector3(0.0f, 0.0f, 0.0f),0.0f, 0.0f);
	Camera SunCamera = Camera(AVector3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f);
	Scene MainScene;

	Texture depthTextureObject;

	// GLFW window object
	GLFWwindow* window = nullptr;
	int windowHeight = 0;
	int windowWidth = 0;
	float windowAspectRatio = 0.0f;

	// Shader
	Shader shaderProgram;
	VAO VAO_1;
	VBO VBO_1;
	EBO EBO_1;

	GLuint instanceVBO;

	GLuint shadowMapLocation;

	GLuint lightDirUnifLoc;

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
	~Engine3D() { EngineTerminate(); };
	// SETERS

	int setupGLFW(const int WINDOW_WIDTH, const int WINDOW_HEIGHT, const char* WINDOW_TITLE);

	void setCamera(double posX, double posY, double posZ);
	void setSunCamera(double posX, double posY, double posZ);

	void setCamera(double posX, double posY, double posZ, float yaw, float pitch);

	const int getDrawStyle(const char* style);

	void setupShaders(const int drawStyle);

	void setupInstanceVBO(const int cntOfObj);

	//void setupShadowMap(const unsigned int SHADOW_WIDTH, const unsigned int SHADOW_HEIGHT);

	inline void setBackground(float R, float G, float B, float A) { backgroundColor = { R,G,B,A }; };

	// GETTERS
	Camera& getCamera(bool Sun);

	Scene* getScene();

	inline int windowShouldClose() { return glfwWindowShouldClose(window); };
	inline Shader& getShader() { return shaderProgram; };

	inline AVertex GetAVertex(double x, double y, double z, int r, int g, int b) {
		float xhigh = (float)x; float xlow = (float)(x - (double)xhigh);
		float zhigh = (float)z; float zlow = (float)(z - (double)zhigh);
		return { xlow, xhigh, zlow, zhigh, (float)y, 
			(float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, 
			0.0f, 0.0f, 0.0f, 0.0f 
		};
	}
	/*
	inline AVertex GetAVertex(float x, float y, float z, int r, int g, int b) {
		return { x, 0.0f, z, 0.0f, y, (float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f };
	}
	*/

	const GLuint GetUniformLocation(const char* uniformName);

	// OTHERS

	void initGameFrame();

	void shadowPass();

	void renderPass(float FOVdeg, float zNear, float zFar, bool UPDATE_VBO);

	//void ConfigureShaderAndMatrices(AVector3& pos);
	//void RenderShadowMap(const unsigned int SHADOW_WIDTH, const unsigned int SHADOW_HEIGHT);

	void EngineTerminate();

	MeshObj* LoadSTLGeomFile(const char *filePath, float scale);

	MeshObj* CreateMesh(const std::vector<AVertex>& vertices, int VertexNumber, const std::vector<int>& indicies, int VertIndexNumber);
	MeshObj* CreatePrism(const std::vector<AVertex>& vertices, int VertexNumber, float height);
	MeshObj* CreateRectPrism(double cx, double cy, double cz, float length, float width, float height);
	MeshObj* CreateCube(double cx, double cy, double cz, float length);

	void DEBUG_showCameraVectors();
};