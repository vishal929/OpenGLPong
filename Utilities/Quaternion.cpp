
// quaternion helper class for 3d rotations

#include "../Includes/Quaternion.hpp";
#include <math.h>

/*Constructor for a quaternion, which represents no rotation*/
Quaternion::Quaternion() {
	// values which represent no rotation
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->w = 1;
}

/*Constructor for a quaternion, provided the axis of rotation and an angle*/
Quaternion::Quaternion(float x, float y, float z, float theta) {
	this->set_quaternion(x, y, z, theta);
}

/*Setting a quaternion given an axis of rotation and an angle*/
void Quaternion::set_quaternion(float x, float y, float z, float theta) {
	this->x = x * sinf(theta / 2);
	this->y = y * sinf(theta / 2);
	this->z = z * sinf(theta / 2);
	this->w = cosf(theta / 2);
}


/*Defining the multiplication between two quaternions: NOT COMMUTATIVE!*/
Quaternion Quaternion::operator*(Quaternion other)
{
	Quaternion product = Quaternion();
	product.w = (other.w * this->w) - (other.x * this->x) - (other.y * this->y) - (other.z * this->z);
	product.x = (this->w * other.x) + (this->x * other.w) + (this->y * other.z) - (this->z * other.y);
	product.y = (this->w * other.y) - (this->x * other.z) + (this->y * other.w) + (this->z * other.x);
	product.z = (this->w * other.z) + (this->x * other.y) - (this->y * other.x) + (this->z * other.w);
	
	return product;
}

/*Defining the norm of a quaternion (this should be 1)*/
float Quaternion::norm() {
	return sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2) + pow(this->w, 2));
}

/*Scale operation for a quaternion (in case we need to renormalize due to floating point errors)*/
void Quaternion::scale(float m) {
	this->x /= m;
	this->y /= m;
	this->z /= m;
	this->w /= m;
}