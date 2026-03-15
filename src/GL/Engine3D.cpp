
#include "Engine3D.h"

Engine3D::~Engine3D() {
	if (shaderProgram) delete shaderProgram;
	if (VAO_1) delete VAO_1;
	if (VBO_1) delete VBO_1;
	if (EBO_1) delete EBO_1;

	if (UserCamera) delete UserCamera;
	if (MainScene) { MainScene->deleteWorldRoot(); delete MainScene; }
}

void Engine3D::setScene() {
	MainScene = new Scene();
}

void Engine3D::setCamera(double posX, double posY, double posZ) {
	Camera_Vector3 Position = { posX, posY, posZ };
	UserCamera = new Camera(Position, -90.0f, 0.0f);
}

void Engine3D::setCamera(double posX, double posY, double posZ, float yaw, float pitch) {
	//EX: Camera({ 0.0f, 80.0f, 120.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f });;
	Camera_Vector3 Position = { posX, posY, posZ };
	UserCamera = new Camera(Position, yaw, pitch);
}

void Engine3D::DEBUG_showCameraVectors() {
	std::cout << UserCamera->Position.x << " " << UserCamera->Position.y << " " << UserCamera->Position.z<<"\n";
	std::cout << UserCamera->Yaw <<" "<< UserCamera->Pitch << "\n";
}

int Engine3D::setupGLFW(const int WINDOW_WIDTH, const int WINDOW_HEIGHT, const char * WINDOW_TITLE) {
	// INITIALIZE GLFW
	glfwInit();
	// SOME SPECS FOR OUR OPENGL VERSION THAT WE SEND TO GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//CORE profile from OPENGL; only modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Create the WINDOW OBJECT with our defined width and height and title
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
	//Error checking if anything went wrong in the window creating process
	if (window == NULL) {
		std::cout << "Error when creating window \n";
		glfwTerminate();
		return 0; //
	}
	//Tell GLFW we are using our created window as it's context
	glfwMakeContextCurrent(window);

	//Load GLAD (needed to configure OpenGL)
	gladLoadGL();
	//Specify Viewport to OpenGL ( from (0,0) to (W_WIDTH,W_HEIGHT) )
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	windowWidth = WINDOW_WIDTH;
	windowHeight = WINDOW_HEIGHT;
	windowAspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
	return 1;
}

void Engine3D::setupShaders() {

	//if (UserCamera == nullptr) { std::cerr << "Unable to setup Shader before Camera Object, please call setCamera() first \n"; return; }

	shaderProgram = new Shader("default.vert", "default.frag");

	UserCamera->camPosUniformLoc = glGetUniformLocation(shaderProgram->ID, "camPosLH");
	UserCamera->perspMat4Loc = glGetUniformLocation(shaderProgram->ID, "perspectiveMatrix");
	UserCamera->camYLoc = glGetUniformLocation(shaderProgram->ID, "camY");

	VAO_1 = new VAO();

	VAO_1->Bind();

	std::vector<AVertex>& worldVertices = MainScene->getVertStoreLocation().getWorldVertices();

	std::vector<GLuint> VertIndicies = MainScene->getVertStoreLocation().getVertIndicies();

	VBO_1 = new VBO( worldVertices.data(), worldVertices.size() * sizeof(AVertex) );
	EBO_1 = new EBO(VertIndicies.data(), VertIndicies.size() * sizeof(GLuint));

	VAO_1->LinkVBO(*VBO_1, 0, 4, GL_FLOAT, 32, (void*)0);
	VAO_1->LinkVBO(*VBO_1, 1, 4, GL_FLOAT, 32, (void*)16);

	VAO_1->Unbind();
	VBO_1->Unbind();
	EBO_1->Unbind();

}

void Engine3D::setupInstanceVBO(const int cntOfObj) {

	// Prepare our matrices, IMPORTANT to set cntOfObj variable to the number of meshes
	modelMatrices.resize(cntOfObj);

	// We create a VBO for matrices of instances
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	// We specify GL_DYNAMIC_DRAW beacause our values will change frequently as the camera moves
	// We need a new relative translation matrix every frame for every instance,
	// Even though our camera view and proj matrix stay the same for every instance
	glBufferData(GL_ARRAY_BUFFER, cntOfObj * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);

	// Configure VAO for mat4 matricies (4*vec4 size)
	VAO_1->Bind();
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

	for (int i = 0; i < 4; i++) {
		glEnableVertexAttribArray(2 + i);
		glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i)); //specify mat4 size
		glVertexAttribDivisor(2 + i, 1); // We tell the GPU the change the matrix PER INSTANCE
	}
	VAO_1->Unbind();

	//Set background color to be drawn
	glClearColor(0.0f, 0.25f, 0.75f, 1.0f);
	//Clear the BACK BUFFER and assign our color to it
	glClear(GL_COLOR_BUFFER_BIT);
	//Swap BACK BUFFER with FRONT BUFFER
	glfwSwapBuffers(window);

}

Camera* Engine3D::getCamera() { return UserCamera; }
Scene* Engine3D::getScene() { return MainScene; }

const GLuint Engine3D::GetUniformLocation(const char* uniformName) {
	return glGetUniformLocation(shaderProgram->ID, uniformName);
}

void Engine3D::initGameFrame() {
	//Set background color to be drawn
	glClearColor(backgroundColor.R, backgroundColor.G, backgroundColor.B, backgroundColor.A);
	//Clear the BACK BUFFER and assign our color to it
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Engine3D::registerCameraInput(float FOVdeg, float zNear, float zFar) {
	UserCamera->Inputs(window);
	UserCamera->Matrix(FOVdeg, zNear, zFar, windowAspectRatio, *shaderProgram);
}

void Engine3D::configureGameFrame(float FOVdeg, float zNear, float zFar) {

	shaderProgram->Activate();
	this->registerCameraInput(FOVdeg, zNear, zFar);

	// Calculate relative positions of all meshes
	// Later, we forward every computed projection matrix to the instanceVBO
	std::vector<MeshObj*>& meshes = MainScene->getMeshes();
	/*
	for (int i = 0; i < modelMatrices.size(); i++) {
		float relX = (float)(meshes[i]->worldPosition.wx - UserCamera->Position.x);
		float relY = (float)(meshes[i]->worldPosition.wy - UserCamera->Position.y);
		float relZ = (float)(meshes[i]->worldPosition.wz - UserCamera->Position.z);
		modelMatrices[i] = UserCamera->mat4Tuple.proj * 
			UserCamera->mat4Tuple.view * 
			glm::translate(glm::mat4(1.0f), glm::vec3(relX, relY, relZ));
	}
	*/

	// We send all matrices to the GPU in a single instance call
	/*
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, modelMatrices.size() * sizeof(glm::mat4), modelMatrices.data());
	*/

	VAO_1->Bind();

	//glDrawElementsInstanced(GL_TRIANGLES, meshes[0]->getSize(),
		//GL_UNSIGNED_INT, 0, modelMatrices.size());

	glDrawElements(GL_TRIANGLES, MainScene->getVertStoreLocation().getVertIndicies().size(), GL_UNSIGNED_INT, 0);
	//Swap BACK BUFFER with FRONT BUFFER
	glfwSwapBuffers(window);
	// Get events (for controls, event handling, closing, etc.)
	glfwPollEvents();
}

void Engine3D::EngineTerminate() {
	//Delete our VAOs, VBOs, EBOs
	VAO_1->Delete();
	VBO_1->Delete();
	EBO_1->Delete();

	//Delete instanceVBO
	glDeleteBuffers(1, &instanceVBO);

	//Delete shader
	shaderProgram->Delete();

	//Destroy WINDOW OBJECT
	glfwDestroyWindow(window);
	//Terminate GLFW
	glfwTerminate();
}

void Engine3D::LoadSTLGeomFile(const char* fileName, int R, int G, int B, float scale) {
	std::vector<float> coords, normals;
	std::vector<unsigned int> tris, solids;

	try {
		stl_reader::ReadStlFile(fileName, coords, normals, tris, solids);

		AVertex* vert = new AVertex[tris.size()];
		int* indicies = new int[tris.size()];

		std::cout <<"Mesh coord count: " << coords.size() << " trig count: " << tris.size()<<"\n";
		const size_t numTris = tris.size() / 3;
		for (size_t itri = 0; itri < numTris; ++itri) {
			//std::cout << "coordinates of triangle " << itri << ": ";
			for (size_t icorner = 0; icorner < 3; ++icorner) {
				int coordINDEX = 3 * tris[3 * itri + icorner];
				int vertexINDEX = 3 * itri + icorner;
				float* c = &coords[coordINDEX];
				indicies[vertexINDEX] = vertexINDEX;
				vert[vertexINDEX] = (this->GetAVertex(c[0]*scale, c[1]*scale, c[2]*scale, R, G, B));
				//std::cout << "(" << c[0] << ", " << c[1] << ", " << c[2] << ") ";
			}
			//std::cout << std::endl;
		}
		MeshObj* createdMesh = new MeshObj(vert, tris.size(), indicies, tris.size(), MainScene);

		delete[] vert;
		delete[] indicies;
		std::cout << "Mesh created \n";
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}