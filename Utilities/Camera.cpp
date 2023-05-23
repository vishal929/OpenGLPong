#include "../Includes/Camera.hpp"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

// camera class for easy camera definitions

/*
	pos is the location in world coordinates to move to
	yaw is a rotation about the x-axis in degrees
	pitch is a rotation about the y-axis in degrees
	roll is a rotation about the z-axis in degrees
*/
void Camera::moveCamera(glm::vec4 pos, float yaw, float pitch, float roll) {
	this->pos = pos;
	
	// creating the yaw rotation matrix
	glm::mat4 yawRot = glm::mat4(1.0f);
	yawRot = glm::rotate(yawRot, glm::radians(yaw), glm::vec3(1.0f,0.0f,0.0f));


	// creating the pitch rotation matrix
	glm::mat4 pitchRot = glm::mat4(1.0f);
	pitchRot = glm::rotate(pitchRot, glm::radians(pitch), glm::vec3(0.0f,1.0f,0.0f));

	// creating the roll rotation matrix
	glm::mat4 rollRot = glm::mat4(1.0f);
	rollRot = glm::rotate(rollRot, glm::radians(roll), glm::vec3(0.0f,0.0f,1.0f));

	// combined XYZ rotation matrix
	glm::mat4 rot = rollRot * pitchRot * yawRot;

	// computing the new directions based on the combined rotation matrix
	this->direction = rot * this->direction;
	this->cameraRight = rot * this->cameraRight;
	this->cameraUp = rot * this->cameraUp;

}

Camera::Camera() {
	this->pos = glm::vec4(0.0f,0.0f,0.0f,1.0f);
	// starting with same axis directions as the world scene
	this->direction = glm::vec4(0.0f,0.0f,1.0f,1.0f);
	this->cameraRight = glm::vec4(1.0f,0.0f,0.0f,1.0f);
	this->cameraUp = glm::vec4(0.0f,1.0f,0.0f,1.0f);
}

glm::mat4 Camera::createView() {
	glm::mat4 posRot(1.0f);
	for (int i = 0; i < 3; i++) {
		posRot[0][i] = this->direction[i];
		posRot[1][i] = this->cameraRight[i];
		posRot[2][i] = this->cameraUp[i];
	}
	glm::mat4 trans = glm::mat4(1.0f);
	// negating the pos so that the scene moves in relation to the camera
	trans = glm::translate(trans, -glm::vec3(this->pos));

	return posRot * trans;
}

