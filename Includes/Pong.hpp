// Pong.hpp header for pong logic including vertex generation, AI, etc.
// PONG_H
#ifndef PONG_H
#define PONG_H

#include "../Includes/Shader.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


/*
	Class which manages the state of our pong game and associated opengl buffers
*/
class PongState {
public:
	
	// objects to render our left bar in pong (the left bar is the player's controllable bar)
	float* leftBarVertices;
	unsigned int leftBarVAO, leftBarVBO, leftBarEBO;
	int leftBarLength;
	
	// objects to render the right bar in pong (the right bar is the AI bar or an opposing player's bar)
	float* rightBarVertices;
	unsigned int rightBarVAO, rightBarVBO, rightBarEBO;
	int rightBarLength;
	
	// objects to render the ball in pong
	float* ballVertices;
	unsigned int ballVAO, ballVBO, ballEBO;
	int ballLength;

	// bar object has a specific height and width (x component is width, y component is height)
	glm::vec2 barDims;

	// ball object has a specific height and width
	glm::vec2 ballDims;


	// storing the positions of the top left coordinate of the bars and the ball (just x,y coordinates in normalized image coordinates)
	glm::vec2 leftBarPos, rightBarPos, ballPos;

	// need to store the velocity vector of the ball (this should be a unit vector)
	glm::vec2 ballVelocity;
	
	// indices used to index the VBO in order to draw rectangles from triangular vertices
	unsigned int* indices;
	int indicesLength;
	
	// float from 0 to 10 which represents a multiplier on the ball speed (10 is 10x faster than 1, 0 is no speed)
	float ballSpeedMultiplier;
	
	// float from 0 to 10 which represents a multiplier on the speed at which a player can move their bar (10 is 10x faster than 1, 0 is no movement)
	float barSpeedMultiplier;

	// left player's score
	int leftScore;

	// right player's score
	int rightScore;

	// maximum score: when a player hits this, they win
	int maxScore;

	// need to keep track of timedelta for non-framerate based movement
	float timeDelta;

	// shader which we want to use for these objects (we use 3 different shaders since we might have 3 different translation transforms)
	Shader* leftBarShader;
	Shader* rightBarShader;
	Shader* ballShader;
	
	/* constructor that initializes vertices and performs opengl setup operations*/
	PongState();
	
	/* function which draws our objects. Should be called inside the rendering loop*/
	void draw(GLFWwindow* window);
	
	/* frees memory and performs cleanup*/
	void destroyState();
	
	/* handle the movement of the player's bar based on up and down arrow keys*/
	void handleMovement(GLFWwindow* window);

	/*
		vertices in 2d representing a bar in pong in normalized coordinates
		We return 6d coordinates where the z dimension is 0 and the last 3 dimensions is the color
		This bar has width 0.04 and height 0.4 in normalized coordinates
	*/
	static float* generateBar();

	/*
		vertices in 2d presenting the ball in pong in normalized coordinates
		We return 4d coordinates where the z dimension is 0 and the last dimension is the color
	*/
	static float* generateBall();
	
	/*
		Indices of vertices to index in order to draw a rectangle from coordinates of a triangle given in a vertex buffer
	*/
	static unsigned int* generateIndices();

	/*
		Setting timedelta for this game object (I will call this in the GLFW render loop)
	*/
	void setTimeDelta(float newDelta);


	

};

// outer callback handler to tie with glfw window
//void bar_outer_callback_handler(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif