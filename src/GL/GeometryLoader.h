#pragma once

#include <glad/glad.h>
#include <vector>
#include "Camera.h"

#define DIVISION_COUNT 2

#define TILE_MAXSCALE_FACTOR 14
#define MAX_TILE_LEVEL 14
#define START_TILE_LEVEL 10

class Tile;
class Scene;
class MeshObj;

struct AVertex {
	float lx, hx;
	float lz, hz;
	float y;
	float r, g, b;
};

class VertexStorage {
private:
	std::vector<AVertex> WorldVertices;
	std::vector<GLuint> vertIndicies;
public:
	VertexStorage() = default;
	~VertexStorage() = default;
	std::vector<GLuint>& getVertIndicies();
	std::vector<AVertex>& getWorldVertices();
	friend class MeshObj;
};

class MeshObj {
private:

	std::vector<GLuint> vertIndicies;
	int meshID = -1; //stays -1 until it is assigned to a scene 
	Scene* scene = nullptr; // which scene it belongs to
public:

	MeshObj(AVertex* vertices, int VertexNumber, int* indicies, int VertIndexNumber, Scene* _scene);
	~MeshObj() = default;
	inline void setMeshID(int _meshID);
	inline int getSize() { return vertIndicies.size(); };
};


class Tile {
private:
	Tile* Parent;
	uint16_t TileX, TileZ;
	uint16_t Level = 0;
	std::vector<int> meshIDs;
public:

	Tile* Divisions[DIVISION_COUNT][DIVISION_COUNT];

	Tile(Tile* _Parent, uint16_t _TileX, uint16_t _TileZ, uint16_t _Level);
	~Tile();
	void DivideTile(uint16_t i, uint16_t j);
};

class Scene {
private:

	Tile* WorldRoot = nullptr;
	std::vector<int> meshIDs;
	std::vector<MeshObj*> Meshes;
	VertexStorage vertStoreLocation;
public:
	Scene();
	~Scene();
	void deleteWorldRoot();
	void AssignMesh(MeshObj* mesh);
	VertexStorage& getVertStoreLocation();
	std::vector<MeshObj*>& getMeshes();
};

