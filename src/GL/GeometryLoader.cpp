#include "GeometryLoader.h"
#include <iostream>
#include <bitset>

int TOTAL_C_COUNT = 0;
int TOTAL_D_COUNT = 0;


const GLfloat scalingFactor = 1.0f;

glm::vec3 getVec3From(AVertex& v) {
	return glm::vec3(v.hx + v.lx, v.y, v.hz + v.lz);
}
void assignNormal(AVertex& v, glm::vec3 N) {
	// Add to already existing normal (if to vertex has been assigned a normal before, initial normal = {0.0f})
	v.nx += N.x; v.ny += N.y; v.nz += N.z;
}

void CalculateSurfaceNormals(int VertIndexNumber, std::vector<AVertex>& worldVert, std::vector<GLuint>& indicies) {
	for (int i = 0; i < VertIndexNumber; i++) {
		AVertex& v = worldVert[indicies[i]];
		v.nx = 0.0f; v.ny = 0.0f; v.nz = 0.0f;
	}
	for (int i = 0; i < (VertIndexNumber/3)*3; i += 3) {
		// We calculate the normal of that triangle face and forward them to every vertex of that face
		AVertex& A = worldVert[indicies[i]];
		AVertex& B = worldVert[indicies[i+1]];
		AVertex& C = worldVert[indicies[i+2]];
		glm::vec3 vectAB = getVec3From(B) - getVec3From(A);
		glm::vec3 vectAC = getVec3From(C) - getVec3From(A);
		glm::vec3 cross = glm::cross(vectAB, vectAC);
		assignNormal(A, cross);
		assignNormal(B, cross);
		assignNormal(C, cross);
	}
	for (int i = 0; i < VertIndexNumber; i++) {
		AVertex& v = worldVert[indicies[i]];
		glm::vec3 finalN = glm::normalize(glm::vec3(v.nx, v.ny, v.nz));
		v.nx = finalN.x; v.ny = finalN.y; v.nz = finalN.z;
	}
}

MeshObj::MeshObj(const std::vector<AVertex>& vertices, int VertexNumber, const std::vector<int>& indicies, int VertIndexNumber, Scene* _scene) {
	std::cout << "C -> Mesh \n";
	
	scene = _scene;

	const float div = 1.0f / (float)VertexNumber;

	Rotation = AVector3(0.0f, 0.0f, 0.0f);
	Size = AVector3(1.0f, 1.0f, 1.0f);
	Position = AVector3(0.0f, 0.0f, 0.0f);
	Color = { 200, 200, 200 };

	center = { 0.0f };
	center.r = (float)Color.R / 255.0f;
	center.g = (float)Color.G / 255.0f;
	center.b = (float)Color.B / 255.0f;

	VertexStorage& vertexStoreLocation = scene->getVertStoreLocation();
	int vertIndiciesCount = (int)vertexStoreLocation.WorldVertices.size();
	for (int i = 0; i < VertexNumber; i++) {
		vertexStoreLocation.WorldVertices.push_back(vertices[i]);
		localVertices.push_back(vertices[i]);
		//std::cout << "Low: X = " << vertices[i].lx << ", Z = " << vertices[i].lz<<"\n";
		//std::cout << "High: X = " << vertices[i].hx << ", Z = " << vertices[i].hz<<"\n\n";
		center.lx += vertices[i].lx * div;
		center.hx += vertices[i].hx * div;
		center.lz += vertices[i].lz * div;
		center.hz += vertices[i].hz * div;
		center.y += vertices[i].y * div;
	}
	verLocation = vertIndiciesCount;
	for (int i = 0; i < VertIndexNumber; i++) {
		vertexStoreLocation.vertIndicies.push_back((GLuint) indicies[i] + vertIndiciesCount);
		this->vertIndicies.push_back((GLuint) indicies[i] + vertIndiciesCount);
	}
	CalculateSurfaceNormals(VertIndexNumber, vertexStoreLocation.WorldVertices, this->vertIndicies);
	scene->AssignMesh(this);

	double cxd = (double)center.lx + (double)center.hx;
	double czd = (double)center.lz + (double)center.hz;
	unsigned int cx = abs( cxd );
	unsigned int cz = abs( czd );
	unsigned int cBitsX = 0x80000000;
	unsigned int cBitsZ = 0x80000000;
	cxd < 0 ? cBitsX = cBitsX - cx : cBitsX = cx & cBitsX;
	czd < 0 ? cBitsZ = cBitsZ - cz : cBitsZ = cz & cBitsZ;

	// Start at bit START_TILE_LEVEL
	// Continue shifting until you reach MAX_TILE_LEVEL
	int lvl = START_TILE_LEVEL;
	unsigned int bin = 1 << (32 - START_TILE_LEVEL);
	Tile* tile = scene->WorldRoot;
	while (lvl < MAX_TILE_LEVEL) {

		short int bitX = (cBitsX & bin) >> (32 - lvl);
		short int bitZ = (cBitsZ & bin) >> (32 - lvl);

		tile = tile->Divisions[bitX][bitZ];

		lvl++;
		bin = bin >> 1;
	}
	if (tile) { 
		tile->meshIDs.push_back(this->meshID); 
	}
	else { 
		std::cout << "tile not found \n"; 
	}
}

void MeshObj::UpdVectors() {
	if (scene == nullptr) { std::cerr << "Mesh does not belong to any scene"; return; }

	center.r = (float)Color.R / 255.0f;
	center.g = (float)Color.G / 255.0f;
	center.b = (float)Color.B / 255.0f;

	glm::mat4 transf = glm::mat4(1.0f);

	transf = glm::rotate(transf, glm::radians((float)Rotation.z), glm::vec3(0, 0, 1));
	transf = glm::rotate(transf, glm::radians((float)Rotation.y), glm::vec3(0, 1, 0));
	transf = glm::rotate(transf, glm::radians((float)Rotation.x), glm::vec3(1, 0, 0));

	transf = glm::scale(transf, glm::vec3(Size.x, Size.y, Size.z));

	transf = glm::translate(transf, glm::vec3(Position.x, Position.y, Position.z));

	auto& WrldVertices = scene->getVertStoreLocation().getWorldVertices();

	glm::vec4 pos(0.0f, 0.0f, 0.0f, 1.0f);
	for (int i = 0; i < vertIndicies.size(); i++) {
		int INDEX = vertIndicies[i];
		AVertex& ver = localVertices[INDEX - verLocation];
		AVertex & worldver = WrldVertices[INDEX];

		pos.x = (ver.lx - center.lx) + (ver.hx - center.hx) - (float)Position.x;
		pos.y = ver.y - center.y - (float)Position.y;
		pos.z = (ver.lz - center.lz) + (ver.hz - center.hz) - (float)Position.z;

		pos = transf * pos;

		double finalX = (double)pos.x + (double)center.lx + (double)center.hx + Position.x;
		double finalY = (double)pos.y + (double)center.y + Position.y;
		double finalZ = (double)pos.z + (double)center.lz + (double)center.hz + Position.z;

		worldver.hx = (float)finalX;
		worldver.lx = (float)(finalX - (double)worldver.hx);

		worldver.hz = (float)finalZ;
		worldver.lz = (float)(finalZ - (double)worldver.hz);

		worldver.y = (float)finalY;

		worldver.r = center.r;
		worldver.g = center.g;
		worldver.b = center.b;

	}

	CalculateSurfaceNormals(vertIndicies.size(), WrldVertices, this->vertIndicies);
}


Tile::Tile(Tile* _Parent, uint16_t _TileX, uint16_t _TileZ, uint16_t _Level) {
	//std::cout << "C tile " << _Level << "\n";
	TOTAL_C_COUNT++;
	//std::cout << "Created: " << TOTAL_C_COUNT << "\n";
	Parent = _Parent; TileX = _TileX; TileZ = _TileZ; Level = _Level;
	for (uint16_t i = 0; i < 2; i++) {
		for (uint16_t j = 0; j < 2; j++) {
			Divisions[i][j] = nullptr;
			if (_Level < MAX_TILE_LEVEL) Tile::DivideTile(i, j);
		}
	}
}

inline void MeshObj::setMeshID(int _meshID) {
	meshID = _meshID;
}

Tile::~Tile() {
	//std::cout << "D tile " << Level << "\n";
	TOTAL_D_COUNT++;
	//std::cout << "Destroyed: " << TOTAL_D_COUNT<<"\n";
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			delete Divisions[i][j];
		}
	}
}

void Tile::DivideTile(uint16_t i, uint16_t j) {
	if (i >= 2 || j >= 2) return;
	if (Divisions[i][j] != nullptr) {
		delete Divisions[i][j];
	}
	Tile* tile = new Tile(this, i, j, Level + 1);
	Divisions[i][j] = tile;
}

Scene::Scene() {
	std::cout << "C -> Scene \n";
	WorldRoot = new Tile(nullptr, 0, 0, START_TILE_LEVEL);
}

Scene::~Scene() {
	if (WorldRoot!=nullptr) delete WorldRoot;
}

void Scene::deleteWorldRoot() {
	if (WorldRoot != nullptr) delete WorldRoot;
}

VertexStorage& Scene::getVertStoreLocation() {
	return vertStoreLocation;
}

void Scene::AssignMesh(MeshObj* mesh) {
	int meshID = (int)meshIDs.size();
	mesh->setMeshID(meshID);
	meshIDs.push_back(meshID);
	Meshes.push_back(mesh);
}

std::vector<MeshObj*>& Scene::getMeshes() {
	return Meshes;
}


std::vector<AVertex>& VertexStorage::getWorldVertices() {
	return this->WorldVertices;
}

std::vector<GLuint>& VertexStorage::getVertIndicies() {
	return this->vertIndicies;
}