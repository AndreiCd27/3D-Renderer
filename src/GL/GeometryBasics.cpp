
#include "GeometryBasics.h"


AVector3 AVector3::operator+(const AVector3& dr) {
	return { dr.x + this->x, dr.y + this->y, dr.z + this->z };
}
AVector3& AVector3::operator+=(const AVector3& dr) {
	this->x += dr.x; this->y += dr.y; this->z += dr.z;
	return *this;
}
AVector3 AVector3::operator*(const AVector3& dr) {
	return { dr.x * this->x, dr.y * this->y, dr.z * this->z };
}
AVector3 AVector3::operator*(const float& scalar) {
	return { scalar * this->x, scalar * this->y, scalar * this->z };
}
AVector3 AVector3::operator^(const AVector3& dr) { //CROSS PRODUCT
	return {
		this->y * dr.z - this->z * dr.y,
		this->z * dr.x - this->x * dr.z,
		this->x * dr.y - this->y * dr.x
	};
}
AVector3 AVector3::Normalize() {
	float dist = std::sqrtf(this->x * this->x + this->y * this->y + this->z * this->z);
	return { this->x / dist, this->y / dist, this->z / dist };
}