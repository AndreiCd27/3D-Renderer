#pragma once

#include "pch.h"
#include "framework.h"


struct AColor3 {
	int R, G, B;
};

// Could have used glm
class AVector3 {
public:
	double x = 0.0f, y = 0.0f, z = 0.0f;
	AVector3() = default;
	AVector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {};
	~AVector3() = default;
	AVector3 operator+(const AVector3& dr);
	AVector3 operator*(const AVector3& dr);
	AVector3 operator*(const float& scalar);
	AVector3& operator+=(const AVector3& dr);
	AVector3 operator^(const AVector3& dr); // Used for cross product
	AVector3 Normalize();
};

struct AVertex { //48 bytes
	float lx, hx;
	float lz, hz;
	float y;
	float r, g, b;
	float nx = 0.0f, ny = 0.0f, nz = 0.0f; //normal vector
	float meshID = 0.0f;
};

struct GeomData {
	std::vector<AVertex> vertices;
	std::vector<GLuint> indicies;
};

// Used to specify a subvector
struct AInterval {
	int start;
	int size;
};