#include <iostream>

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const char* WINDOW_TITLE = "window";
const float aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

#include "Engine3D.h"

// Vertices coordinates
std::vector<AVertex> VERTICES;

int main() {

	Engine3D engine;
	std::cout << "Engine initialized \n";

	int success = engine.setupGLFW(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
	if (!success) { std::cerr << "Error at setup \n"; return -1; }

	engine.setBackground(0.2f, 0.3f, 0.8f, 1.0f);

	// HERE WE CREATE OUR OBJECTS /////////////////////////////////////////////////

	VERTICES.push_back(engine.GetAVertex(-300.0f, -50.0f, -300.0f, 0, 255, 0));
	VERTICES.push_back(engine.GetAVertex(-300.0f, -50.0f, 200.0f, 0, 255, 0));
	VERTICES.push_back(engine.GetAVertex(200.0f, -50.0f, 200.0f, 0, 255, 0));
	VERTICES.push_back(engine.GetAVertex(200.0f, -50.0f, -300.0f, 0, 255, 0));

	MeshObj* plane = engine.CreatePrism(VERTICES, 4, 50.0f);

	VERTICES.clear();

	VERTICES.push_back(engine.GetAVertex(-25.0f, 0.0f, -25.0f, 0, 0, 255));
	VERTICES.push_back(engine.GetAVertex(-25.0f, 0.0f, 25.0f, 0, 0, 255));
	VERTICES.push_back(engine.GetAVertex(25.0f, 0.0f, 25.0f, 0, 0, 255));
	VERTICES.push_back(engine.GetAVertex(25.0f, 0.0f, -25.0f, 0, 0, 255));

	MeshObj* plane2 = engine.CreatePrism(VERTICES, 4, 80.0f);

	VERTICES.clear();

	VERTICES.push_back(engine.GetAVertex(25.0f, 0.0f, 25.0f, 255, 0, 255));
	VERTICES.push_back(engine.GetAVertex(25.0f, 0.0f, 75.0f, 255, 0, 255));
	VERTICES.push_back(engine.GetAVertex(75.0f, 0.0f, 75.0f, 255, 0, 255));
	VERTICES.push_back(engine.GetAVertex(75.0f, 0.0f, 25.0f, 255, 0, 255));

	MeshObj* plane3 = engine.CreatePrism(VERTICES, 4, 50.0f);

	VERTICES.clear();

	MeshObj * humanMesh = engine.LoadSTLGeomFile("BASEmodel.stl", 20.0f);
	if (humanMesh) {
		std::cout << "Created: Human \n";
		humanMesh->Position = AVector3(-60.0f, 50.0f, 5.0f);
		humanMesh->UpdVectors();
	}

	MeshObj* cube = engine.CreateCube(0.0f, 0.0f, 0.0f, 80.0f);
	cube->Color = { 255, 255, 0 };
	cube->UpdVectors();

	////////////////////////////////////////////////////////////////////////////////

	std::cout << "Meshes constructed \n";

	engine.setCamera(0.0f, 80.0f, 120.0f); //Set camera to this position
	engine.setSunCamera(0.0f, 100.0f, 1.0f);

	engine.setupShaders(engine.getDrawStyle("dynamic")); //Uses Camera Class and Mesh Instances

	std::cout << "Shaders created\n";
	
	//engine.setupInstanceVBO(cntOfObj);

	
	//glDisable(GL_CULL_FACE);

	float _sceneRotationDEG = 0.0f;
	float _deltaTimeForTIMER = 1.0f / 10.0f;
	double prevTime = glfwGetTime();

	int ROT = 0;
	long long int frameCounter = 0;

	// MAIN GAME LOOP
	while (!engine.windowShouldClose()) {

		engine.initGameFrame(); // setting up the background color

		// TIMER (global) ////////////
		double currentTime = glfwGetTime();
		if (currentTime - prevTime >= _deltaTimeForTIMER) {
			//engine.DEBUG_showCameraVectors();
			prevTime = currentTime;
			if (humanMesh) {
				humanMesh->Rotation = AVector3((-(int)sqrt(ROT*20) % 360 - 180) * 1.0f, 
					(ROT % 360 - 180) * 1.0f, (-ROT % 360 - 180) *1.0f);
				humanMesh->UpdVectors();
			}
			double sinROT = sin((double)ROT / 1024.0f);
			double cosROT = cos((double)ROT / 1024.0f);
			double sinYAW = sin((double)ROT / 1024.0f);
			double cosYAW = cos((double)ROT / 1024.0f);
			engine.getCamera(true).Position = { 100.0f * cosROT, 100.0f * sinROT, 50.0f };
			float lightReflactance = std::max(0.0f, sinf((float)ROT / 512.0f));
			float sunsetCoef = abs(cos((double)ROT / 512.0f)*1.5f);
			engine.setBackground((sunsetCoef) * 0.25f * (lightReflactance + 0.25f), 
				(lightReflactance + sunsetCoef / 2.0f) * 0.5f * (lightReflactance), 
				lightReflactance, 1.0f);
			ROT+=4;
			frameCounter++;
		}

		engine.shadowPass();
		engine.renderPass(45.0f, 0.1f, 1000.0f, true);
	}

	return 0;
}