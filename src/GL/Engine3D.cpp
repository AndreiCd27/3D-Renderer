
#include "Engine3D.h"


void Engine3D::setCamera(double posX, double posY, double posZ) {
	AVector3 Position = { posX, posY, posZ };
	UserCamera = Camera(Position, -90.0f, 0.0f);
}

void Engine3D::setCamera(double posX, double posY, double posZ, float yaw, float pitch) {
	//EX: Camera({ 0.0f, 80.0f, 120.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f });;
	AVector3 Position = { posX, posY, posZ };
	UserCamera = Camera(Position, yaw, pitch);
}

void Engine3D::setSunCamera(double posX, double posY, double posZ) {
	//EX: Camera({ 0.0f, 80.0f, 120.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f });;
	AVector3 Position = { posX, posY, posZ };
	SunCamera = Camera(Position, 0.0f, 0.0f);
}


void Engine3D::DEBUG_showCameraVectors() {
	std::cout << UserCamera.Position.x << " " << UserCamera.Position.y << " " << UserCamera.Position.z<<"\n";
	std::cout << UserCamera.Yaw <<" "<< UserCamera.Pitch << "\n";
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

const int Engine3D::getDrawStyle(const char* style) {
	if (style == "static") return GL_STATIC_DRAW;
	if (style == "stream") return GL_STREAM_DRAW;
	if (style == "dynamic") return GL_DYNAMIC_DRAW;
	return GL_STATIC_DRAW;
}

void Engine3D::setupShaders(const int drawStyle=GL_STATIC_DRAW) {

	//if (UserCamera == nullptr) { std::cerr << "Unable to setup Shader before Camera Object, please call setCamera() first \n"; return; }

	shaderProgram.Setup("default.vert", "default.frag");

	UserCamera.camPosUniformLoc = glGetUniformLocation(shaderProgram.ID, "camPosLH");
	UserCamera.perspMat4Loc = glGetUniformLocation(shaderProgram.ID, "perspectiveMatrix");
	UserCamera.camYLoc = glGetUniformLocation(shaderProgram.ID, "camY");

	SunCamera.perspMat4Loc = glGetUniformLocation(shaderProgram.ID, "lightPerspMatrix");
	SunCamera.userPerspMat4Loc = glGetUniformLocation(shaderProgram.ID, "perspectiveMatrix");

	lightDirUnifLoc = glGetUniformLocation(shaderProgram.ID, "lightDirection");

	VAO_1.Setup();

	VAO_1.Bind();

	std::vector<AVertex>& worldVertices = MainScene.getVertStoreLocation().getWorldVertices();

	std::vector<GLuint>& VertIndicies = MainScene.getVertStoreLocation().getVertIndicies();

	VBO_1.Setup(worldVertices.data(), worldVertices.size() * sizeof(AVertex), drawStyle );
	EBO_1.Setup(VertIndicies.data(), VertIndicies.size() * sizeof(GLuint), drawStyle );

	size_t stride = sizeof(AVertex); //48 bytes
	VAO_1.LinkVBO(VBO_1, 0, 4, GL_FLOAT, stride, (void*)0);
	VAO_1.LinkVBO(VBO_1, 1, 4, GL_FLOAT, stride, (void*)16);
	VAO_1.LinkVBO(VBO_1, 2, 4, GL_FLOAT, stride, (void*)32);

	depthTextureObject.setupFBO();
	depthTextureObject.setupDepthTexture(2048, shadowMapLocation);

	VAO_1.Unbind();
	VBO_1.Unbind();
	EBO_1.Unbind();

	glEnable(GL_DEPTH_TEST);

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
	VAO_1.Bind();
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

	for (int i = 0; i < 4; i++) {
		glEnableVertexAttribArray(2 + i);
		glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i)); //specify mat4 size
		glVertexAttribDivisor(2 + i, 1); // We tell the GPU the change the matrix PER INSTANCE
	}
	VAO_1.Unbind();

	//Set background color to be drawn
	glClearColor(0.0f, 0.25f, 0.75f, 1.0f);
	//Clear the BACK BUFFER and assign our color to it
	glClear(GL_COLOR_BUFFER_BIT);
	//Swap BACK BUFFER with FRONT BUFFER
	glfwSwapBuffers(window);

}

Camera& Engine3D::getCamera(bool Sun) { if (Sun) { return SunCamera; } return UserCamera; }
Scene* Engine3D::getScene() { return &MainScene; }

const GLuint Engine3D::GetUniformLocation(const char* uniformName) {
	return glGetUniformLocation(shaderProgram.ID, uniformName);
}

void Engine3D::initGameFrame() {
	//Set background color to be drawn
	glClearColor(backgroundColor.R, backgroundColor.G, backgroundColor.B, backgroundColor.A);
}

void Engine3D::registerCameraInput(float FOVdeg, float zNear, float zFar) {
	UserCamera.Inputs(window);
	UserCamera.Matrix(FOVdeg, zNear, zFar, windowAspectRatio, shaderProgram);
}

void Engine3D::configureGameFrame(float FOVdeg, float zNear, float zFar, bool UPDATE_VBO) {

	glBindFramebuffer(GL_FRAMEBUFFER, depthTextureObject.FBO_ID);
	glViewport(0, 0, 2048, 2048);
	glClear(GL_DEPTH_BUFFER_BIT);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	shaderProgram.Activate();
	VAO_1.Bind();

	glUniform1f(UserCamera.camYLoc, 0);
	glUniform4f(UserCamera.camPosUniformLoc, 0, 0, 0, 0);

	SunCamera.LightMatrix(500.0f, shaderProgram, SunCamera.perspMat4Loc, SunCamera.userPerspMat4Loc, false);

	// The Sun is looking from it's Position to the center (0,0,0);
	glUniform3f(lightDirUnifLoc, SunCamera.Position.x, SunCamera.Position.y, SunCamera.Position.z);

	if (UPDATE_VBO) {
		glBindBuffer(GL_ARRAY_BUFFER, VBO_1.ID);
		auto& VBOdata = MainScene.getVertStoreLocation().getWorldVertices();
		glBufferSubData(GL_ARRAY_BUFFER, 0, VBOdata.size() * sizeof(AVertex), VBOdata.data());
	}


	int indiciesSize = MainScene.getVertStoreLocation().getVertIndicies().size();

	glDrawElements(GL_TRIANGLES, indiciesSize, GL_UNSIGNED_INT, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);
	glViewport(0, 0, windowWidth, windowHeight);
	//Clear the BACK BUFFER and assign our color to it
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glCullFace(GL_BACK);

	shaderProgram.Activate();
	VAO_1.Bind();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	this->registerCameraInput(FOVdeg, zNear, zFar);

	SunCamera.LightMatrix(500.0f, shaderProgram, SunCamera.perspMat4Loc, 0, true);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTextureObject.depthTexture);
	//glUniform1i(shadowMapLocation, 0);

	glDrawElements(GL_TRIANGLES, indiciesSize, GL_UNSIGNED_INT, 0);
	//Swap BACK BUFFER with FRONT BUFFER
	glfwSwapBuffers(window);
	// Get events (for controls, event handling, closing, etc.)
	glfwPollEvents();
}

void Engine3D::EngineTerminate() {
	//Delete our VAOs, VBOs, EBOs
	VAO_1.Delete();
	VBO_1.Delete();
	EBO_1.Delete();

	//Delete instanceVBO
	glDeleteBuffers(1, &instanceVBO);

	//Delete shader
	shaderProgram.Delete();

	//Destroy WINDOW OBJECT
	glfwDestroyWindow(window);
	//Terminate GLFW
	glfwTerminate();
}

MeshObj* Engine3D::LoadSTLGeomFile(const char* fileName, int R, int G, int B, float scale) {
	std::vector<float> coords, normals;
	std::vector<unsigned int> tris, solids;

	try {
		stl_reader::ReadStlFile(fileName, coords, normals, tris, solids);

		std::vector<AVertex> vert;
		std::vector<int> indicies;

		const size_t totalIndices = tris.size();
		vert.reserve(totalIndices);
		indicies.reserve(totalIndices);

		std::cout <<"Mesh coord count: " << coords.size() << " trig count: " << tris.size()<<"\n";
		const size_t numTris = tris.size() / 3;
		for (size_t itri = 0; itri < numTris; ++itri) {
			//std::cout << "coordinates of triangle " << itri << ": ";
			for (size_t icorner = 0; icorner < 3; ++icorner) {
				int coordINDEX = 3 * tris[3 * itri + icorner];
				int vertexINDEX = 3 * itri + icorner;
				float* c = &coords[coordINDEX];
				indicies.push_back(vertexINDEX);
				vert.push_back(this->GetAVertex(c[0]*scale, c[1]*scale, c[2]*scale, 
					R + (c[0] * scale * 10 / 2), G - (c[2] * scale * 10 / 4), B + (c[2]*scale * 10)));
				//std::cout << "(" << c[0] << ", " << c[1] << ", " << c[2] << ") ";
			}
			//std::cout << std::endl;
		}

		std::cout << "Mesh created \n";

		return new MeshObj(vert, (int)tris.size(), indicies, (int)tris.size(), &MainScene);;
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return nullptr;
}

MeshObj* Engine3D::CreateMesh(const std::vector<AVertex>& vertices, int VertexNumber, 
	const std::vector<int>& indicies, int VertIndexNumber) {

	if (vertices.size() < 3) { std::cout << "Mesh does not contain any triangles \n"; return nullptr; };

	return new MeshObj(vertices, VertexNumber, indicies, VertIndexNumber, &MainScene);
}
MeshObj* Engine3D::CreatePrism(const std::vector<AVertex>& vertices, int VertexNumber, float height) {

	std::vector<int> indicies;

	std::vector<AVertex> V = vertices;
	V.reserve(VertexNumber * 2);

	if (vertices.size() < 3) { std::cout << "Mesh does not contain any triangles \n"; return nullptr; };

	// 0 -> 1 -> 2
	// |  / |  / |
	// | /  | /  |
	// 3 -> 4 -> 5

	for (int i = 0; i < VertexNumber; i++) {
		AVertex vclone = vertices[i];
		vclone.y += height;
		V.push_back(vclone); // create bottom vertex
	}
	for (int i = 0; i < VertexNumber-1; i++) {
		//LATERAL FACE 1
		indicies.push_back(i);
		indicies.push_back(i + 1);
		indicies.push_back(VertexNumber + i);
		//LATERAL FACE 2
		indicies.push_back(VertexNumber + i + 1);
		indicies.push_back(VertexNumber + i);
		indicies.push_back(i + 1);
	}

	//LATERAL FACE 1
	indicies.push_back(VertexNumber - 1);
	indicies.push_back(0);
	indicies.push_back(2 * VertexNumber - 1);
	//LATERAL FACE 2
	indicies.push_back(VertexNumber);
	indicies.push_back(2 * VertexNumber - 1);
	indicies.push_back(0);

	for (int i = 1; i < VertexNumber-1; i++) {
		//BOTTOM FACE
		indicies.push_back(0);
		indicies.push_back(i);
		indicies.push_back(i + 1);
		
		//TOP FACE
		indicies.push_back(VertexNumber);
		indicies.push_back(VertexNumber + i);
		indicies.push_back(VertexNumber + i + 1);
	}

	return new MeshObj(V, VertexNumber*2, indicies, indicies.size(), &MainScene);
}