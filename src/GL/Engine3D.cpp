#include "pch.h"
#include "framework.h"

#include "Engine3D.h"
#include <cstring>

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
	if (strcmp(style, "static") == 0) return GL_STATIC_DRAW;
	if (strcmp(style, "stream") == 0) return GL_STREAM_DRAW;
	if (strcmp(style, "dynamic") == 0) return GL_DYNAMIC_DRAW;
	return GL_STATIC_DRAW;
}

void Engine3D::setupShaders(const int drawStyle=GL_STATIC_DRAW) {

	//if (UserCamera == nullptr) { std::cerr << "Unable to setup Shader before Camera Object, please call setCamera() first \n"; return; }

	shaderProgram.Setup("default.vert", "default.frag");

	//std::cout << "Shader setup complete! \n";

	UserCamera.camPosUniformLoc = glGetUniformLocation(shaderProgram.ID, "camPosLH");
	UserCamera.perspMat4Loc = glGetUniformLocation(shaderProgram.ID, "perspectiveMatrix");
	UserCamera.camYLoc = glGetUniformLocation(shaderProgram.ID, "camY");

	SunCamera.perspMat4Loc = glGetUniformLocation(shaderProgram.ID, "lightPerspMatrix");
	SunCamera.userPerspMat4Loc = glGetUniformLocation(shaderProgram.ID, "perspectiveMatrix");

	lightDirUnifLoc = glGetUniformLocation(shaderProgram.ID, "lightDirection");

	//std::cout << "VAO Setup \n";
	VAO_1.Setup();

	VAO_1.Bind();
	//std::cout << "VAO Setup and Binding complete \n";

	std::vector<AVertex>& worldVertices = MainScene.getVertStoreLocation().getWorldVertices();

	std::vector<GLuint>& VertIndicies = MainScene.getVertStoreLocation().getVertIndicies();

	//std::cout << "Got vertex and indicies buffers \n";

	VBO_1.Setup(worldVertices, worldVertices.size() * sizeof(AVertex), drawStyle );
	EBO_1.Setup(VertIndicies, VertIndicies.size() * sizeof(GLuint), drawStyle );

	//std::cout << "VBO & EBO setup complete \n";

	GLsizei stride = sizeof(AVertex); //48 bytes
	VAO_1.LinkVBO(VBO_1, 0, 4, GL_FLOAT, stride, (void*)0);
	VAO_1.LinkVBO(VBO_1, 1, 4, GL_FLOAT, stride, (void*)16);
	VAO_1.LinkVBO(VBO_1, 2, 4, GL_FLOAT, stride, (void*)32);

	//std::cout << "VBO linking complete \n";

	depthTextureObject.setupFBO();
	depthTextureObject.setupDepthTexture(2048, shadowMapLocation);

	//std::cout << "Setup FBO complete \n";

	VAO_1.Unbind();
	VBO_1.Unbind();
	EBO_1.Unbind();

	//std::cout << "Unbinding..\n";

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

void Engine3D::loadChunks(Tile* from) {

	//std::cout << "Getting meshes! \n";

	std::vector<MeshObj*> meshes = MainScene.GetTileMeshes(from);

	//std::cout << "Loaded meshes! \n";
	
	DATA_3D_PAIR.vert.clear();
	DATA_3D_PAIR.indicies.clear();

	//std::cout << "Getting global verticies \n";

	std::vector<AVertex>& globalVertices = MainScene.getVertStoreLocation().getWorldVertices();

	//std::cout << "Loaded global verticies \n";

	for (MeshObj* Mesh : meshes) {
		for (GLuint i : Mesh->vertIndicies) {
			DATA_3D_PAIR.vert.push_back(globalVertices[i]);
			DATA_3D_PAIR.indicies.push_back(DATA_3D_PAIR.indicies.size());
		}
	}

	//std::cout << "Got pairs! \n";
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

void Engine3D::shadowPass() {
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

	size_t indiciesSize = MainScene.getVertStoreLocation().getVertIndicies().size();

	glDrawElements(GL_TRIANGLES, indiciesSize, GL_UNSIGNED_INT, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);

}

void Engine3D::renderPass(float FOVdeg, float zNear, float zFar, bool UPDATE_VBO) {

	
	if (UPDATE_VBO) {

		//

		loadChunks(MainScene.WorldRoot);

		//std::cout << "Loaded pairs! \n";
		//

		glBindBuffer(GL_ARRAY_BUFFER, VBO_1.ID);
		auto& VBOdata = DATA_3D_PAIR.vert; //MainScene.getVertStoreLocation().getWorldVertices();
		size_t VBOsize = VBOdata.size();
		//std::cout << "VBO size: " << VBOsize << " | VBO Capacity: " << VBO_1.Capacity << "\n";
		if (VBOsize > VBO_1.Capacity) {
			VBO_1.Capacity *= 2;
			glBufferData(GL_ARRAY_BUFFER, VBO_1.Capacity * sizeof(AVertex), VBOdata.data(), GL_DYNAMIC_DRAW);
		} else {
			glBufferData(GL_ARRAY_BUFFER, VBO_1.Capacity * sizeof(AVertex), NULL, GL_DYNAMIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, VBOsize * sizeof(AVertex), VBOdata.data());
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_1.ID);
		auto& EBOdata = DATA_3D_PAIR.indicies; //MainScene.getVertStoreLocation().getVertIndicies();
		size_t EBOsize = EBOdata.size();
		//std::cout << "EBO size: " << EBOsize << " | EBO Capacity: " << EBO_1.Capacity << "\n";
		if (EBOsize > EBO_1.Capacity) {
			EBO_1.Capacity *= 2;
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, EBO_1.Capacity * sizeof(GLuint), EBOdata.data(), GL_DYNAMIC_DRAW);
		}
		else {
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, EBO_1.Capacity * sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, EBOdata.size() * sizeof(GLuint), EBOdata.data());
		}

		//std::cout << EBOdata.size() << " " << VBOdata.size() << " INDICIES | VERTEX LOADS \n";
	}

	size_t indiciesSize = MainScene.getVertStoreLocation().getVertIndicies().size();

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

	//Delete meshes
	for (auto& MeshObj : MainScene.getMeshes()) {
		delete MeshObj;
	}

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

MeshObj* Engine3D::LoadSTLGeomFile(const char* fileName, float scale) {
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
		const int numTris = tris.size() / 3;
		for (int itri = 0; itri < numTris; ++itri) {
			//std::cout << "coordinates of triangle " << itri << ": ";
			for (int icorner = 0; icorner < 3; ++icorner) {
				int coordINDEX = 3 * tris[3 * itri + icorner];
				int vertexINDEX = 3 * itri + icorner;
				float* c = &coords[coordINDEX];
				indicies.push_back(vertexINDEX);
				vert.push_back(this->GetAVertex(c[0]*scale, c[1]*scale, c[2]*scale, 200, 200, 200));
				//std::cout << "(" << c[0] << ", " << c[1] << ", " << c[2] << ") ";
			}
			//std::cout << std::endl;
		}

		std::cout << "Mesh created \n";

		return new MeshObj(vert, (int)tris.size(), indicies, (int)tris.size(), &MainScene);
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

MeshObj* Engine3D::CreateRectPrism(double cx, double cy, double cz, float length, float width, float height) {
	std::vector<AVertex> v;
	v.resize(4);
	v[0] = GetAVertex(cx - length / 2.0f, cy - height / 2.0f, cz - width / 2.0f, 200, 200, 200);
	v[1] = GetAVertex(cx - length / 2.0f, cy - height / 2.0f, cz + width / 2.0f, 200, 200, 200);
	v[2] = GetAVertex(cx + length / 2.0f, cy - height / 2.0f, cz + width / 2.0f, 200, 200, 200);
	v[3] = GetAVertex(cx + length / 2.0f, cy - height / 2.0f, cz - width / 2.0f, 200, 200, 200);
	return CreatePrism(v, 4, height);
}

MeshObj* Engine3D::CreateCube(double cx, double cy, double cz, float length) {
	return CreateRectPrism(cx, cy, cz, length, length, length);
}