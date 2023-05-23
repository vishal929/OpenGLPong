
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

/*Getting a rotation matrix from a quaternion*/
glm::mat4 Quaternion::getRotationMatrix()
{
	glm::mat4 ret = glm::mat4(1.0f);
	// adjusting the top left block part of the 4x4 identity matrix to get our rotation matrix
	ret[0][0] = 1- 2 * pow(y,2) - 2 * pow(z,2);
	ret[0][1] = 2 * x * y - 2 * w * z;
	ret[0][2] = 2 * x * z + 2 * w * y;
	ret[1][0] = 2 * x * y + 2 * w * z;
	ret[1][1] = 1 - 2 * pow(x,2) - 2 * pow(z,2);
	ret[1][2] = 2 * y * z + 2 * w * x;
	ret[2][0] = 2 * x * z - 2 * w * y;
	ret[2][1] = 2 * y * z - 2 * w * x;
	ret[2][2] = 1 - 2 * pow(x,2) - 2 * pow(y,2);
}

