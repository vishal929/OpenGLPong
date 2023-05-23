// camera_h
#ifndef CAMERA_H
#define CAMERA_H

// using the glm library for math operations
#include<glm/glm.hpp>

class Camera {
	public:
	/* A camera has a position specified in world coordinates*/
	glm::vec4 pos;

	/*A camera has a direction vector (looking at a spot in the world scene) (this is the z-axis of the camera space).
	Note that this direction will be the negative z-axis of the camera space!*/
	glm::vec4 direction;

	/*A camera has an x-axis*/
	glm::vec4 cameraRight;

	/*A camera has a y-axis*/
	glm::vec4 cameraUp;
	
	/*Constructor of a camera, given a camera position in the world space and a location in the world space to look at*/
	Camera();
	
	/*
	pos is the location in world coordinates to move to
	yaw is a rotation about the x-axis in degrees
	pitch is a rotation about the y-axis in degrees
	roll is a rotation about the z-axis in degrees
	*/
	void moveCamera(glm::vec4 pos, float yaw, float pitch, float roll);
	
	/*
		Moving the camera and generating direction vectors with quaternions instead of euler angles
		pos is the location in world coordinates to move to
		yaw is a rotation about the x-axis in degrees
		pitch is a rotation about the y-axis in degrees
		roll is a rotation about the z-axis in degrees

	*/
	void moveCameraQuaternion(glm::vec4 pos, float yaw, float pitch, float roll);

	/*Returns a view matrix, which is the transformation which transforms coordinates in the world space to the camera space*/
	glm::mat4 createView();

};


#endif
