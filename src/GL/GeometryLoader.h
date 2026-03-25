#pragma once

#include "pch.h"
#include "framework.h"

#include "MultiArray.h"
#include "Camera.h"
#include "VBO.h"
#include "EBO.h"

#define TILE_MAXSCALE_FACTOR 14
#define MAX_TILE_LEVEL 14
#define START_TILE_LEVEL 10

class Blueprint;
class Instance;
class Tile;
class Scene;
class MeshObj;

class BlueprintException : public std::runtime_error {
public:
	BlueprintException(const std::string& Msg, int errorCode) :
		std::runtime_error("[ BLUEPRINT_ERROR ] -> ERR_" + std::to_string(errorCode) + " : " + Msg) {};
};

class Blueprint {
	static const int safeBlueprintCount; // Throws an exception if we go beyond 4095 blueprints
	// If any modifications are to be made, consider looking into the shiftComponent variable
	// In the Tile class and modify it such that no conflicts arise
	// At the moment, the shift component is calculated as follows: (int) log2l(Blueprint::safeBlueprintCount) + 1;

	static int contor; // Used to set templateID, simple incremented variable, starting at 0

	GeomData Geometry; // Stores the vertices and indicies of the blueprint, any instance of this blueprint
	// Has this same geometry, but with different Position, Rotation and Scale

	int templateID; // TemplateID is used in combination with TileID to form a HandleID
	// HandleID will be the key to access data shared between tiles and blueprints 
	// (like transformation matricies)
	// Only 4095 Blueprints may be created, the rest 20 bits are reserved for tileIDs
	// A simple OR (|) operation can generate the key we need for Blueprint A in Tile B 
	// ( HandleID = A | ( B << 12 ) )
public:
	AVertex center;
	Blueprint(const std::vector<AVertex>& vertices, const std::vector<GLuint>& indicies);

	static const int GetShiftComponent();
};

class MeshVertexObject {
protected:
	VBO MeshVBO;
	EBO MeshEBO;
	GeomData Geometry;
public:

};

class Instance {
	int instanceID = -1;
	AVector3 Rotation; // Rotation in degrees for X,Y,Z axis
	AVector3 Size; // Scale Vector for X,Y,Z axis
	AVector3 Position; // Translate Vector for X,Y,Z axis
	AColor3 Color;
	const Blueprint* Template;
public:
	Instance(const Blueprint* Template);
	friend Scene;
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
	AVertex center;

	std::vector<AVertex> localVertices;

	static MeshVertexObject MVO;

	int verLocation;

	int meshID = -1; // Stays -1 until it is assigned to a scene 
	Scene* scene; // Which scene it belongs to
	Tile* whatTile = nullptr;
	int whereInTile = -1; //Where our mesh is in the tile meshIDs vector

	bool visible = false;
public:
	std::vector<GLuint> vertIndicies;
	AVector3 Rotation; // Rotation in degrees for X,Y,Z axis
	AVector3 Size; // Scale Vector for X,Y,Z axis
	AVector3 Position; // Translate Vector for X,Y,Z axis
	AColor3 Color;

	void FindMeInTile();
	void UpdVectors(); // Call this function whenever you want to update the verticies with Rotation and Size

	MeshObj(const std::vector<AVertex>& vertices, size_t VertexNumber, const std::vector<int>& indicies, int VertIndexNumber, Scene* _scene);
	~MeshObj() { localVertices.clear(); vertIndicies.clear(); };
	inline void setMeshID(int _meshID);
	inline size_t getSize() { return vertIndicies.size(); };
};


class Tile {
private:
	static const int shiftComponent;

	Tile* Parent;
	uint16_t TileX, TileZ;
	uint16_t Level = 0;
public:

	// Each tile/chunk has empty VBO objects, and we setup them dynamically
	// Based on the user's preferences (more on that later)
	// At MAX_TILE_LEVEL, we automaticall setup the VBOs

	// InstanceVBO: here we store transformation matricies (mat4)
	// Calculated for each instance according to:
	// Position, Rotation, Scale vectors
	// Additional color channel may be included
	// Useful for instances that need to be replicated thousands of times
	// And have the same geometry, but different transform matricies
	// Thus, they only need ONE EBO, but every instance TYPE
	// Must have their own EBO, so avoid a lot of instance types,
	// As this makes the engine have to do more draw calls
	// I recommend 50-100 instance types <=> 50-100 draw calls
	// Think of: trees, cubes, car models, particles etc.
	// PROs: 
	//  - many, many instances with less GPU calls, so better performance
	//  - very good for dynamic instances that update almost every frame
	//  - dynamic tile reallocation, moving the object moves the refrence to it
	//  from the previous tile to another tile
	// CONs: 
	//	- can have too many draw calls if there are many instance types
	//  - can't manipulate individual verticies, only transformations are allowed
	AInterval tileInterval; // This specifies a subarray from the global instanceVBO
	// Only instances that belong to this tile will be drawn by specifying the start
	// And size of our matrix subarray
	// We use glDrawElementsInstancedBaseVertexBaseInstance

	// UniqueVBO: here we store mesh objects that are replicated less
	// The vertex data and vertex indicies are stored directly into
	// VertexStorage, so updating transform vectors may be expensive
	// BUT, we can have thousands of different mesh types with
	// different geometry
	// Thus, unique meshes should be created once and copied less
	// Think of: complex meshes, high-resolution items, etc.
	// PROs:
	//  - best for unique meshes that are sparsely located in the map
	//  - can have complex geometry and allows many types of objects
	//  - good performance for mostly static (but not always static) meshes
	//  - dynamic tile reallocation, moving the object moves the refrence to it
	//  from the previous tile to another tile
	// CONs:
	//  - meshes can have overhead if constantly updated and transformed,
	//  - beacause every vertex must be updated
	//  - not very space efficient
	VBO UniqueVBO;
	GeomData UniqueData;

	// StaticVBO: here we store meshes that can't be updates
	// Uses GL_STATIC_DRAW, and can have complex geometry per chunk
	// Does not have transform vectors such as Position, Rotation, Scale
	// Individual verticies may be changed
	// Think of: terrain, mountains, buildings etc.
	// Can't be reallocated to another tile, completly chunk-static
	// PROs:
	//  - best for static objects
	//  - can have complex geometry, good performance
	//  - user can manipulate individual verticies
	// CONs:
	//  - can't be transformed/moved
	//  - requires a lot of storing space
	//  - does require an EBO per chunk/tile
	//  - vertex modifications are complex
	VBO StaticVBO;
	EBO StaticEBO;
	GeomData StaticData;

	std::vector<int> meshIDs;
	Tile* Divisions[2][2] = { nullptr };

	Tile(Tile* _Parent, uint16_t _TileX, uint16_t _TileZ, uint16_t _Level);
	~Tile();
	void DivideTile(uint16_t i, uint16_t j);
	uint16_t getLevel() { return Level; }
};

class Scene {
private:

	VBO instanceVBO; // Here we bind the transformation matricies for our instances
	VBO geomVBO; // Here we bind the vertices for our geometry objects
	EBO geomEBO; // Here we bind the vertex indicies for our geometry objects

	// Handles are generated for every Blueprint and for every Tile
	ArrayOrganizer<glm::mat4> instanceOrganizer;

	// Handles are generated for every Tile
	ArrayOrganizer<AVertex> VBO_Organizer;
	ArrayOrganizer<GLuint> EBO_Organizer;

	std::vector<int> meshIDs;
	std::vector<MeshObj*> Meshes;
	VertexStorage vertStoreLocation;
public:

	Tile* WorldRoot = nullptr;

	Scene();
	~Scene();
	void deleteWorldRoot();
	void RecurseInTiles(std::vector<MeshObj*>& TileMeshes, Tile* tile);
	std::vector<MeshObj*> GetTileMeshes(Tile* startTile);
	void AssignMesh(MeshObj* mesh);
	VertexStorage& getVertStoreLocation();
	std::vector<MeshObj*>& getMeshes();

	Tile* FindTileForPosition(AVertex center, AVector3 Position);

	Instance* CreateInstance(Blueprint* temp, AVector3 pos);
};