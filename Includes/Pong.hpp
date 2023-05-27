// Pong.hpp header for pong logic including vertex generation, AI, etc.
// PONG_H
#ifndef PONG_H
#define PONG_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../Includes/Shader.hpp"

/*
	Class which manages the state of our pong game and associated opengl buffers
*/
class PongState {
public:
	
	// objects to render our left bar in pong
	float* leftBarVertices;
	unsigned int leftBarVAO, leftBarVBO, leftBarEBO;
	int leftBarLength;
	
	// objects to render the right bar in pong
	float* rightBarVertices;
	unsigned int rightBarVAO, rightBarVBO, rightBarEBO;
	int rightBarLength;
	
	// objects to render the ball in pong
	float* ballVertices;
	unsigned int ballVAO, ballVBO, ballEBO;
	int ballLength;
	
	// indices used to index the VBO in order to draw rectangles from triangular vertices
	unsigned int* indices;
	int indicesLength;
	
	// float from 0 to 10 which represents a multiplier on the ball speed (10 is 10x faster than 1, 0 is no speed)
	float ballSpeedMultiplier;
	
	// float from 0 to 10 which represents a multiplier on the speed at which a player can move their bar (10 is 10x faster than 1, 0 is no movement)
	float barSpeedMultiplier;

	// shader which we want to use for these objects
	Shader* shader;
	
	/* constructor that initializes vertices and performs opengl setup operations*/
	PongState();
	
	/* function which draws our objects. Should be called inside the rendering loop*/
	void draw();
	
	/* frees memory and performs cleanup*/
	void destroyState();

	/*
		vertices in 2d representing the left player's bar in pong in normalized coordinates
		We return 6d coordinates where the z dimension is 0 and the last 3 dimensions is the color
	*/
	static float* generateLeftBar();

	/*
		vertices in 2d representing the right player's bar in pong in normalized coordinates
		We return 4d coordinates where the z dimension is 0 and the last dimension is the color
	*/
	static float* generateRightBar();

	/*
		vertices in 2d presenting the ball in pong in normalized coordinates
		We return 4d coordinates where the z dimension is 0 and the last dimension is the color
	*/
	static float* generateBall();
	
	/*
		Indices of vertices to index in order to draw a rectangle from coordinates of a triangle given in a vertex buffer
	*/
	static unsigned int* generateIndices();
};


#endif