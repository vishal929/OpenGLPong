// quaternion_h

#ifndef QUATERNION_H
#define QUATERNION_H

class Quaternion {
public:
	// x-position of axis of rotation
	float x;

	// y-position of axis of rotation
	float y;

	// z-position of axis of rotation
	float z;

	// rotation strength
	float w;

	/*Constructor for a quaternion, which represents no rotation*/
	Quaternion();

	/*Constructor for a quaternion, provided the axis of rotation and an angle*/
	Quaternion(float x, float y, float z, float theta);

	/*Setting a quaternion given an axis of rotation and an angle*/
	void set_quaternion(float x, float y, float z, float theta);

	/*Defining the multiplication between two quaternions: NOT COMMUTATIVE!*/
	Quaternion operator*(Quaternion other);

	/*Defining the norm of a quaternion (this should be 1)*/
	float norm();

	/*Scale operation for a quaternion (in case we need to renormalize due to floating point errors)*/
	void scale(float m);
};
#endif