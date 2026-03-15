#include <iostream>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const char* WINDOW_TITLE = "window";
const float aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

#include "Engine3D.h"

// Vertices coordinates
std::vector<AVertex> VERTICES;


// Indices for vertices order
int indicies[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4
};

int main() {

	Engine3D engine;
	int success = engine.setupGLFW(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
	if (!success) { std::cerr << "Error at setup \n"; return -1; }

	engine.setScene();

	// HERE WE CREATE OUR OBJECTS /////////////////////////////////////////////////

	VERTICES.push_back(engine.GetAVertex(-25.0f, 0.0f, -25.0f, 255, 0, 0));
	VERTICES.push_back(engine.GetAVertex(-25.0f, 0.0f, 25.0f, 0, 0, 0));
	VERTICES.push_back(engine.GetAVertex(25.0f, 0.0f, 25.0f, 0, 255, 0));
	VERTICES.push_back(engine.GetAVertex(25.0f, 0.0f, -25.0f, 0, 0, 0));
	VERTICES.push_back(engine.GetAVertex(0.0f, 30.0f, 0.0f, 0, 0, 255));

	int vertCount = VERTICES.size();
	int indexCount = sizeof(indicies) / sizeof(int);
	constexpr int cntOfObj = 16;
	MeshObj* pyramids[cntOfObj];
	for (int i = 0; i < cntOfObj; i++) {
		AVertex* vert = VERTICES.data();
		for (int k = 0; k < VERTICES.size(); k++) {
			vert[k].lx += 50.0f;
		}
		pyramids[i] = new MeshObj(vert, vertCount, indicies, indexCount, engine.getScene());
	}

	////////////////////////////////////////////////////////////////////////////////

	engine.setCamera(0.0f, 80.0f, 120.0f); //Set camera to this position

	engine.setupShaders(); //Uses Camera Class and Mesh Instances

	//engine.setupInstanceVBO(cntOfObj);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	float _sceneRotationDEG = 0.0f;
	float _deltaTimeForTIMER = 1.0f / 2.0f;
	double prevTime = glfwGetTime();

	// MAIN GAME LOOP
	while (!engine.windowShouldClose()) {
		
		engine.initGameFrame();

		//engine.registerCameraInput(45.0f, 0.1f, 1000.0f);

		// TIMER (global) ////////////
		double currentTime = glfwGetTime();
		if (currentTime - prevTime >= _deltaTimeForTIMER) {
			//engine.DEBUG_showCameraVectors();
			prevTime = currentTime;
		}

		engine.configureGameFrame(45.0f, 0.1f, 1000.0f);
	}

	for (int i = 0; i < cntOfObj; i++) {
		delete pyramids[i];
	}

	engine.EngineTerminate();

	return 0;
}