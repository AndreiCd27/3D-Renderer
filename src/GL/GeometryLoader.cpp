#include "GeometryLoader.h"
#include <iostream>

int TOTAL_C_COUNT = 0;
int TOTAL_D_COUNT = 0;


const GLfloat scalingFactor = 1.0f;

MeshObj::MeshObj(AVertex * vertices, int VertexNumber, int* indicies, int VertIndexNumber, Scene* _scene) {
	scene = _scene;
	VertexStorage& vertexStoreLocation = scene->getVertStoreLocation();
	int vertIndiciesCount = (int)vertexStoreLocation.WorldVertices.size();
	for (int i = 0; i < VertexNumber; i++) {
		vertexStoreLocation.WorldVertices.push_back(vertices[i]);
		//std::cout << "Low: X = " << vertices[i].lx << ", Z = " << vertices[i].lz<<"\n";
		//std::cout << "High: X = " << vertices[i].hx << ", Z = " << vertices[i].hz<<"\n\n";
	}
	for (int i = 0; i < VertIndexNumber; i++) {
		vertexStoreLocation.vertIndicies.push_back((GLuint) indicies[i] + vertIndiciesCount);
		this->vertIndicies.push_back((GLuint) indicies[i] + vertIndiciesCount);
	}
	scene->AssignMesh(this);
}

Tile::Tile(Tile* _Parent, uint16_t _TileX, uint16_t _TileZ, uint16_t _Level) {
	//std::cout << "C tile " << _Level << "\n";
	TOTAL_C_COUNT++;
	//std::cout << "Created: " << TOTAL_C_COUNT << "\n";
	Parent = _Parent; TileX = _TileX; TileZ = _TileZ; Level = _Level;
	for (uint16_t i = 0; i < DIVISION_COUNT; i++) {
		for (uint16_t j = 0; j < DIVISION_COUNT; j++) {
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
	for (int i = 0; i < DIVISION_COUNT; i++) {
		for (int j = 0; j < DIVISION_COUNT; j++) {
			delete Divisions[i][j];
		}
	}
}

void Tile::DivideTile(uint16_t i, uint16_t j) {
	if (i >= DIVISION_COUNT || j >= DIVISION_COUNT) return;
	if (Divisions[i][j] != nullptr) {
		delete Divisions[i][j];
	}
	Tile* tile = new Tile(this, i, j, Level + 1);
	Divisions[i][j] = tile;
}

Scene::Scene() {
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