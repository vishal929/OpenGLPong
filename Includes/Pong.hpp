// Pong.hpp header for pong logic including vertex generation, AI, etc.
// PONG_H
#ifndef PONG_H
#define PONG_H

#include "../Includes/Shader.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <random>


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

	// objects to render scoreboards in pong
	float* leftScoreFirstDigitVertices; float* leftScoreSecondDigitVertices;
	unsigned int leftScoreFirstDigitVAO, leftScoreFirstDigitVBO, leftScoreFirstDigitEBO;
	unsigned int leftScoreSecondDigitVAO, leftScoreSecondDigitVBO, leftScoreSecondDigitEBO;
	int leftScoreLength;

	float* rightScoreFirstDigitVertices; float* rightScoreSecondDigitVertices;
	unsigned int rightScoreFirstDigitVAO, rightScoreFirstDigitVBO, rightScoreFirstDigitEBO;
	unsigned int rightScoreSecondDigitVAO, rightScoreSecondDigitVBO, rightScoreSecondDigitEBO;
	int rightScoreLength;

	unsigned int scoreTexture;

	// bar object has a specific height and width (x component is width, y component is height)
	glm::vec2 barDims;

	// ball object has a specific height and width
	glm::vec2 ballDims;

	// each digit of the scoreboard has a specific height and width
	glm::vec2 scoreDigitDims;


	// storing the positions of the top left coordinate of the bars and the ball (just x,y coordinates in normalized image coordinates)
	// we need the last position of the ball to help with collision detection
	glm::vec2 leftBarPos, rightBarPos, ballPos, ballLastPos;

	// need to score the top left coordinate of the left digit of the scoreboards for each player
	glm::vec2 leftScorePos, rightScorePos;

	// need to store the velocity vector of the ball (this should be a unit vector when not at rest)
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
	Shader* leftScoreFirstDigitShader;
	Shader* leftScoreSecondDigitShader;
	Shader* rightScoreFirstDigitShader;
	Shader* rightScoreSecondDigitShader;
	
	/* constructor that initializes vertices and performs opengl setup operations*/
	PongState();
	
	/* function which draws our objects. Should be called inside the rendering loop*/
	void draw(GLFWwindow* window);

	/*Function which returns game status
	  If 0, then the game is still in progress
	  If 1, then the left player has won
	  If 2, then the right player has won
	*/
	int gameStatus();
	
	/*
		Set some values which we need between games
	*/
	void setGameParameters(float ballSpeed, float barSpeed, int maxScore);
	
	/* frees memory and performs cleanup*/
	void destroyState();
	
	/* handle the movement of the player's bar based on up and down arrow keys*/
	void handleMovement(GLFWwindow* window);
	
	/* handle the movement update for the ball and any possible collisions
	   We return 1 if there is any goal (to indicate that we need to reset or check for end of game status)
	   We return 0 if there is no goal -> the game keeps going
	*/
	int handleBallMovement();

	/* handle the movement of the AI to hit the ball*/
	void handleAIMovement();
	
	/* Reset state of the game after a score
	   If totalReset is true, then we restart the game from scratch (zero score on both sides)
	*/
	void resetGame(bool totalReset);

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
		vertices in 2d representing a quad we can map a bitmap font to 
		this way we can simulate a score by using bitmap resources
		digit is a value from 0 to 9 from which we generate texture coordinates to the corresponding digit in the bitmap font
	*/
	static float* generateScoreBoard(int digit);
	
	/*
		adjusts the texture coordinates in the current scoreboard buffer to account for the new score	
	*/
	static void adjustScoreBoard(float* scoreBoard, int digit);
	
	/*
		Indices of vertices to index in order to draw a rectangle from coordinates of a triangle given in a vertex buffer
	*/
	static unsigned int* generateIndices();

	/*
		Setting timedelta for this game object (I will call this in the GLFW render loop)
	*/
	void setTimeDelta(float newDelta);

	/*
		Helper function which sets the ball's initial velocity vector to a random direction
	*/
	void setBallInitialDirection();

	/*
		Setup random distributions to sample from
	*/
	void setupDistribution() {
		dist = std::uniform_real_distribution<float>(0.0f, 1.0f);
	}
	
	/*
		Sampling a random number from our distribution between 0 and 1
	*/
	float sampleRandom() {
		return dist(generator);
	}

private:
	// random generator to use for random intialization of a ball direction
	std::default_random_engine generator;

	// uniform distribution between 0 and 1 for initializing the random direction of the ball
	std::uniform_real_distribution<float> dist;
	
		

};

// outer callback handler to tie with glfw window
//void bar_outer_callback_handler(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif