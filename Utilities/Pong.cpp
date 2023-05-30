#include "../Includes/Pong.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ctime>
#include <random>
#include "../Includes/stb_image.h"
#include <iostream>

// imgui for a UI interface I can use
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
// Pong.cpp holds logic for building and running the pong game (AI, vertices, etc.)

PongState::PongState()
{
    // initializing the score
    leftScore = 0;
    rightScore = 0;

    maxScore = 3;

    // we start with default settings in the state
    ballSpeedMultiplier = 1;
    barSpeedMultiplier = 5;

    // setting up shaders to use with our pong state
    leftBarShader = new Shader("Vertex_Shaders/color_shader.vs", "Fragment_Shaders/color_shader.fs");
    rightBarShader = new Shader("Vertex_Shaders/color_shader.vs", "Fragment_Shaders/color_shader.fs");
    ballShader = new Shader("Vertex_Shaders/color_shader.vs", "Fragment_Shaders/color_shader.fs");    
    leftScoreFirstDigitShader = new Shader("Vertex_Shaders/matrix_shader.vs", "Fragment_Shaders/texture_shader.fs");
    leftScoreSecondDigitShader = new Shader("Vertex_Shaders/matrix_shader.vs", "Fragment_Shaders/texture_shader.fs");
    rightScoreFirstDigitShader = new Shader("Vertex_Shaders/matrix_shader.vs", "Fragment_Shaders/texture_shader.fs");
    rightScoreSecondDigitShader = new Shader("Vertex_Shaders/matrix_shader.vs", "Fragment_Shaders/texture_shader.fs");

	// generating vertices
	leftBarLength = 24, rightBarLength = 24, ballLength = 24, leftScoreLength=32, rightScoreLength=32;
	leftBarVertices = generateBar();
	rightBarVertices = generateBar();
	ballVertices = generateBall();

    barDims = glm::vec2(leftBarVertices[6]-leftBarVertices[0],leftBarVertices[2*6+1]-leftBarVertices[1]);
    ballDims = glm::vec2(ballVertices[6]-ballVertices[0],ballVertices[2*6+1]-ballVertices[1]);
    
    // scores start at 0
    leftScoreFirstDigitVertices = generateScoreBoard(0);
    leftScoreSecondDigitVertices = generateScoreBoard(0);
    rightScoreFirstDigitVertices = generateScoreBoard(0);
    rightScoreSecondDigitVertices = generateScoreBoard(0);

    scoreDigitDims = glm::vec2(leftScoreFirstDigitVertices[8] - leftScoreFirstDigitVertices[0], 
                                leftScoreFirstDigitVertices[2 * 8 + 1] - leftScoreFirstDigitVertices[1]);


    // initializing positions as the top left vertex of each object with even distances
    leftBarPos = glm::vec2(-0.95f,0.2f);
    rightBarPos = glm::vec2(0.91f,0.2f);
    ballPos = glm::vec2(-0.02f,0.02f);
    ballLastPos = glm::vec2(-0.02f, 0.02f);

    leftScorePos = glm::vec2(-0.4, 0.7);
    rightScorePos = glm::vec2(0.4, 0.7);

    // initializing the random distribution to sample from
    setupDistribution();

    // randomize ball velocity vector direction to start
    setBallInitialDirection();

	// indices to draw rectangles from triangle coordinates
	indices = generateIndices();
	indicesLength = 6;

	// opengl setup for all our VAOs for all our objects
    glGenVertexArrays(1, &leftBarVAO);
    glGenVertexArrays(1, &rightBarVAO);
    glGenVertexArrays(1, &ballVAO);
    glGenVertexArrays(1, &leftScoreFirstDigitVAO);
    glGenVertexArrays(1, &leftScoreSecondDigitVAO);
    glGenVertexArrays(1, &rightScoreFirstDigitVAO);
    glGenVertexArrays(1, &rightScoreSecondDigitVAO);


    // setting up the left bar first
    glBindVertexArray(leftBarVAO);
    
    // initialize VBO ids
    glGenBuffers(1, &leftBarVBO);
    glGenBuffers(1, &rightBarVBO);
    glGenBuffers(1, &ballVBO);
    glGenBuffers(1, &leftScoreFirstDigitVBO);
    glGenBuffers(1, &leftScoreSecondDigitVBO);
    glGenBuffers(1, &rightScoreFirstDigitVBO);
    glGenBuffers(1, &rightScoreSecondDigitVBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, leftBarVBO);
    glBufferData(GL_ARRAY_BUFFER, leftBarLength * sizeof(float), leftBarVertices, GL_DYNAMIC_DRAW);

    // initialize our EBO ids (we are drawing rectangles so lets draw by indices in the EBO)
    glGenBuffers(1, &leftBarEBO);
    glGenBuffers(1, &rightBarEBO);
    glGenBuffers(1, &ballEBO);
    glGenBuffers(1, &leftScoreFirstDigitEBO);
    glGenBuffers(1, &leftScoreSecondDigitEBO);
    glGenBuffers(1, &rightScoreFirstDigitEBO);
    glGenBuffers(1, &rightScoreSecondDigitEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, leftBarEBO);
    // static draw since the indices do not change, only the vertices might change
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    
    // declaring attributes
    glEnableVertexAttribArray(0); //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(1); //color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));

    // setting up right bar attributes
    glBindVertexArray(rightBarVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rightBarVBO);
    glBufferData(GL_ARRAY_BUFFER, rightBarLength * sizeof(float), rightBarVertices, GL_DYNAMIC_DRAW); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rightBarEBO);
    // static draw since the indices do not change, only the vertices might change
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    
    // declaring attributes
    glEnableVertexAttribArray(0); //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(1); //color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));

    // setting up ball attributes
    glBindVertexArray(ballVAO);
    glBindBuffer(GL_ARRAY_BUFFER, ballVBO);
    glBufferData(GL_ARRAY_BUFFER, ballLength * sizeof(float), ballVertices, GL_DYNAMIC_DRAW); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ballEBO);
    // static draw since the indices do not change, only the vertices might change
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    
    // declaring attributes
    glEnableVertexAttribArray(0); //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(1); //color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));

    

    // setting up left score first digit attributes
    glBindVertexArray(leftScoreFirstDigitVAO);
    glBindBuffer(GL_ARRAY_BUFFER, leftScoreFirstDigitVBO);
    glBufferData(GL_ARRAY_BUFFER, leftScoreLength * sizeof(float), leftScoreFirstDigitVertices, GL_DYNAMIC_DRAW); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, leftScoreFirstDigitEBO);
    // static draw since the indices do not change, only the vertices might change
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    
    // declaring attributes
    glEnableVertexAttribArray(0); //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(1); //color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float))); 
    glEnableVertexAttribArray(2); //texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float))); 

    // loading a texture we will need for our scores
    glGenTextures(1, &scoreTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, scoreTexture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // loading our texture
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load("Textures/characters.bmp", &width, &height, &nrChannels, 0);

    if (data == nullptr) {
        // we failed to load the texture
        std::cout << "FAILED::LOADING::TEXTURE!" << std::endl;
        return;
    }

    // generating texture and mipmap based on loaded image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // we can free the data, we have the mipmaps generated
    stbi_image_free(data);

    // setting up left score second digit attributes
    glBindVertexArray(leftScoreSecondDigitVAO);
    glBindBuffer(GL_ARRAY_BUFFER, leftScoreSecondDigitVBO);
    glBufferData(GL_ARRAY_BUFFER, leftScoreLength * sizeof(float), leftScoreSecondDigitVertices, GL_DYNAMIC_DRAW); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, leftScoreSecondDigitEBO);
    // static draw since the indices do not change, only the vertices might change
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    
    // declaring attributes
    glEnableVertexAttribArray(0); //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(1); //color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float))); 
    glEnableVertexAttribArray(2); //texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float))); 

    // setting up right score first digit attributes
    glBindVertexArray(rightScoreFirstDigitVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rightScoreFirstDigitVBO);
    glBufferData(GL_ARRAY_BUFFER, rightScoreLength * sizeof(float), rightScoreFirstDigitVertices, GL_DYNAMIC_DRAW); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rightScoreFirstDigitEBO);
    // static draw since the indices do not change, only the vertices might change
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    
    // declaring attributes
    glEnableVertexAttribArray(0); //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(1); //color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float))); 
    glEnableVertexAttribArray(2); //texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float))); 

    // setting up right score second digit attributes
    glBindVertexArray(rightScoreSecondDigitVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rightScoreSecondDigitVBO);
    glBufferData(GL_ARRAY_BUFFER, rightScoreLength * sizeof(float), rightScoreSecondDigitVertices, GL_DYNAMIC_DRAW); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rightScoreSecondDigitEBO);
    // static draw since the indices do not change, only the vertices might change
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesLength * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    
    // declaring attributes
    glEnableVertexAttribArray(0); //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(1); //color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float))); 
    glEnableVertexAttribArray(2); //texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float))); 

}

void PongState::draw(GLFWwindow* window)
{
    // handling movement
    handleMovement(window);
    handleAIMovement();
    int isGoal = handleBallMovement();
    //printf("left score: %d ----- right score: %d \n", leftScore, rightScore);

    // drawing the score
    int leftScoreLeftDigit, leftScoreRightDigit;
    if (leftScore < 9) {
        leftScoreLeftDigit = 0;
        leftScoreRightDigit = leftScore;
    }
    else {
        leftScoreLeftDigit = leftScore / 10;
        leftScoreRightDigit = leftScore % 10;
    }
    leftScoreFirstDigitShader->use();
    // binding the texture used for the score
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE0, scoreTexture);
    // sending uniform for translation
    glm::mat4 translation = glm::mat4(1.0f);
    translation = glm::translate(translation, glm::vec3(leftScorePos, 0.0f));
    int transformLoc = glGetUniformLocation(leftScoreFirstDigitShader->ID, "transformation");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(translation));
    // sending uniform for texture shift
    transformLoc = glGetUniformLocation(leftScoreFirstDigitShader->ID, "textureShift");
    glUniform2f(transformLoc, leftScoreLeftDigit * 0.0627f, 0);
	glBindVertexArray(leftScoreFirstDigitVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

    leftScoreSecondDigitShader->use();
    // sending uniform for translation
    translation = glm::mat4(1.0f);
    translation = glm::translate(translation, glm::vec3(leftScorePos + glm::vec2(scoreDigitDims.x,0.0f), 0.0f));
    transformLoc = glGetUniformLocation(leftScoreSecondDigitShader->ID, "transformation");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(translation));
    // sending uniform for texture shift
    transformLoc = glGetUniformLocation(leftScoreSecondDigitShader->ID, "textureShift");
    glUniform2f(transformLoc, leftScoreRightDigit * 0.0627f, 0);
	glBindVertexArray(leftScoreSecondDigitVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
    
    int rightScoreLeftDigit, rightScoreRightDigit;
    if (rightScore < 9) {
        rightScoreLeftDigit = 0;
        rightScoreRightDigit = rightScore;
    }
    else {
        rightScoreLeftDigit = rightScore / 10;
        rightScoreLeftDigit = rightScore % 10;
    }
    rightScoreFirstDigitShader->use();
    // sending uniform for translation
    translation = glm::mat4(1.0f);
    translation = glm::translate(translation, glm::vec3(rightScorePos , 0.0f));
    transformLoc = glGetUniformLocation(rightScoreFirstDigitShader->ID, "transformation");
    // sending uniform for texture shift to adjust digit
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(translation));
    // sending uniform for texture shift
    transformLoc = glGetUniformLocation(rightScoreFirstDigitShader->ID, "textureShift");
    glUniform2f(transformLoc, rightScoreLeftDigit * 0.0627f, 0);
	glBindVertexArray(rightScoreFirstDigitVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

    rightScoreSecondDigitShader->use();
    // sending uniform for translation
    translation = glm::mat4(1.0f);
    translation = glm::translate(translation, glm::vec3(rightScorePos + glm::vec2(scoreDigitDims.x,0.0f) , 0.0f));
    transformLoc = glGetUniformLocation(rightScoreSecondDigitShader->ID, "transformation");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(translation));
    // sending uniform for texture shift
    transformLoc = glGetUniformLocation(rightScoreSecondDigitShader->ID, "textureShift");
    glUniform2f(transformLoc, rightScoreRightDigit * 0.0627f, 0);
	glBindVertexArray(rightScoreSecondDigitVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

    leftBarShader->use();
    // sending uniform for translation
    translation = glm::mat4(1.0f);
    translation = glm::translate(translation, glm::vec3(leftBarPos, 0.0f));
    transformLoc = glGetUniformLocation(leftBarShader->ID, "transformation");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(translation));
	glBindVertexArray(leftBarVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    rightBarShader->use();
    // sending uniform for translation
    translation = glm::mat4(1.0f);
    translation = glm::translate(translation, glm::vec3(rightBarPos, 0.0f));
    transformLoc = glGetUniformLocation(rightBarShader->ID, "transformation");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(translation));
	glBindVertexArray(rightBarVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    ballShader->use();
    // sending uniform for translation
    translation = glm::mat4(1.0f);
    translation = glm::translate(translation, glm::vec3(ballPos, 0.0f));
    transformLoc = glGetUniformLocation(ballShader->ID, "transformation");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(translation));
	glBindVertexArray(ballVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

    


    if (isGoal) {
            resetGame(false);
    }
}

void PongState::destroyState()
{
    delete(leftBarVertices);
    delete(rightBarVertices);
    delete(ballVertices);
    delete(indices);
    delete(leftBarShader);
    delete(rightBarShader);
    delete(ballShader);
}

/*
	vertices in 2d representing a bar in pong in normalized coordinates
	We return 6d coordinates where the z dimension is 0 and the last 3 dimensions is the color
    This bar has height 0.4 and width 0.04 in normalized coordinates
*/
float* PongState::generateBar()
{
    float* vertices = new float[24]{
		  0.0f,-0.4f,0.00f,1.0f,0.0f,0.0f, // bottom left of rectangle
		  0.04f,-0.4f, 0.00f, 1.0f, 0.0f, 0.0f,// bottom right of rectangle
		  0.0f,0.0f,0.00f,1.0f, 0.0f, 0.0f, // top left of rectangle
		  0.04f,0.0f,0.00f,1.0f, 0.0f, 0.0f // top right of rectangle

	};
    
	return vertices;
}

/*
	vertices in 2d presenting the ball in pong in normalized coordinates
	We return 4d coordinates where the z dimension is 0 and the last dimension is the color
    This ball is a square of sidelength 0.04 in normalized coordinates
*/
float* PongState::generateBall()
{
    float* vertices = new float[24]{
		 0.0f,-0.04f,0.00f,1.0f,0.0f,0.0f, // bottom left of rectangle
		 0.04f,-0.04f, 0.00f, 1.0f, 0.0f, 0.0f,// bottom right of rectangle
		 0.0f,0.0f,0.00f,1.0f, 0.0f, 0.0f, // top left of rectangle
		 0.04f,0.0f,0.00f,1.0f, 0.0f, 0.0f // top right of rectangle

	}; 
    
	return vertices;
}

/*
    generating a quad we can use a bitmap font on to simulate a score
    digit is a value from 0 to 9 from which we generate texture coordinates to the corresponding digit in the bitmap font
*/
float* PongState::generateScoreBoard(int digit)
{
    float* vertices = new float[32]{
         // vertices      // color       // texture coordinates
		 0.0f,-0.1f,0.00f,1.0f,1.0f,1.0f, digit * 0.0627f, 0.75f,// bottom left of rectangle
		 0.1f,-0.1f, 0.00f, 1.0f, 1.0f, 1.0f, (digit+1) * 0.0627f, 0.75f,// bottom right of rectangle
		 0.0f,0.0f,0.00f,1.0f, 1.0f, 1.0f,digit * 0.0627f,0.75f + 0.0627f,  // top left of rectangle
		 0.1f,0.0f,0.00f,1.0f, 1.0f, 1.0f, (digit+1)*0.0627f, 0.75f+ 0.0625f // top right of rectangle
	};
    return vertices;
}

int PongState::gameStatus()
{
    if (leftScore == maxScore) {
        return 1;
    }
    else if (rightScore == maxScore) {
        return 2;
    } 
    return 0;
}

/* adjusts the scoreboard buffer to account for a new digit*/
void PongState::adjustScoreBoard(float* scoreBoard, int digit)
{
    // bottom left
    scoreBoard[6] = 0.25f + 0.0625f; 
    scoreBoard[7] = digit * 0.0625f;
    // bottom right
    scoreBoard[14] = 0.25f + 0.0625f; 
    scoreBoard[15] = (digit+1) * 0.0625f;
    // top left
    scoreBoard[22] = 0.25f;
    scoreBoard[23] = digit * 0.0625f;
    // top right
    scoreBoard[30] = 0.25f;
    scoreBoard[31] = (digit + 1) * 0.0625f;
}

unsigned int* PongState::generateIndices()
{
	unsigned int* indices = new unsigned int[6] {
		0,1,2, // first triangle
		2,3,1  // second triange
	};
	return indices;
}

int PongState::handleBallMovement()
{
   // update the balls state and account for collisions 
    // each collision will make the ball faster in the collision component of the velocity!
    ballLastPos.x = ballPos.x;
    ballLastPos.y = ballPos.y;

    ballPos.x += ballVelocity.x * timeDelta * ballSpeedMultiplier;
    ballPos.y += ballVelocity.y * timeDelta * ballSpeedMultiplier;

    // check for collisions and reset ball if necessary (after scoring)

    // checking first for collisions with the boundary (recall that ballPos is the position of thet top left of the ball)
    // for collisions, we reset the position to the boundary and reverse the component of the velocity based on the collision
    if (ballPos.x <= -1.0f) {
        // goal state! this is a goal for the right player (hit the left wall)
        rightScore += 1;
        return 1;
    }
    else if (ballPos.x + ballDims.x >= 1.0f) {
        // goal state! this is a goal for the left player (hit the right wall)
        leftScore += 1;
        return 1;
    }
    else if (ballPos.y - ballDims.y <= -1.0f) {
        // hit the bottom wall
        ballPos.y = -1.0f + ballDims.y;
        ballVelocity.y *= -1;
    }
    else if (ballPos.y >= 1.0f) {
        // hit the top wall
        ballPos.y = 1.0f;
        ballVelocity.y *= -1;
    }

    // check for collision with any bars

    // check collision with left bar
    if ((ballPos.x >= leftBarPos.x && ballPos.x <= leftBarPos.x + barDims.x) &&
         (ballPos.y >= leftBarPos.y-barDims.y && ballPos.y  <= leftBarPos.y)) {
            
        // we have a collision with the left bar, we have to determine which components to reverse
        // we can determine this by checking the last position of the ball (which must be outside the bounding box) 
        if (ballLastPos.y > leftBarPos.y && ballLastPos.x < leftBarPos.x + barDims.x) {
            // collision with the top of the bar
            ballPos.y = leftBarPos.y;
            ballVelocity.y *= -1;
        }
        else if (ballLastPos.y > leftBarPos.y - barDims.y && ballLastPos.x > leftBarPos.x + barDims.x) {
            // collision with the side of the bar
            ballPos.x = leftBarPos.x + barDims.x;
            
            // we adjust the y velocity based on where on the paddle the ball collides
            float hitDist = (leftBarPos.y - barDims.y / 2) - ballPos.y;
            // getting a float between 0 and 1
            hitDist /= barDims.y / 2;
            ballVelocity.y = ballVelocity.x * hitDist;

            ballVelocity.x *= -1;
            
            
        }
        else if (ballLastPos.y < leftBarPos.y - barDims.y && ballLastPos.x > leftBarPos.x){
            // collision with the bottom of the bar
            ballPos.y = leftBarPos.y - barDims.y;
            ballVelocity.y *= -1;
        }
        else if (ballLastPos.y > leftBarPos.y && ballLastPos.x > leftBarPos.x + barDims.x) {
            // top right corner collision
            ballPos.y = leftBarPos.y;
            ballPos.x = leftBarPos.x + barDims.x;
            ballVelocity *= -1;
        }
        else if (ballLastPos.y < leftBarPos.y - barDims.y && ballLastPos.x > leftBarPos.x + barDims.x) {
            // bottom right corner collision
            ballPos.x = leftBarPos.x + barDims.x;
            ballPos.y = leftBarPos.y - barDims.y;
            ballVelocity *= -1;
        }
    }

    // check collision with right bar

    // need to move the ball slightly, since we only record the top left of the ball
    glm::vec2 modifiedBallPos = glm::vec2(0.0f, 0.0f);
    modifiedBallPos.x = ballPos.x + ballDims.x;
    modifiedBallPos.y = ballPos.y;
    if ((modifiedBallPos.x >= rightBarPos.x && modifiedBallPos.x <= rightBarPos.x + barDims.x) &&
         (modifiedBallPos.y >= rightBarPos.y-barDims.y && modifiedBallPos.y  <= rightBarPos.y)) {
        
        // need to move the ball slightly for right collision, since we only record the top left of the ball
        glm::vec2 modifiedLastBallPos = glm::vec2(0.0f, 0.0f);
        modifiedLastBallPos.x = ballLastPos.x + ballDims.x;
        modifiedLastBallPos.y = ballLastPos.y;
        // we have a collision with the right bar, we have to determine which components to reverse
        // we can determine this by checking the last position of the ball (which must be outside the bounding box) 
        if (modifiedLastBallPos.y > rightBarPos.y && modifiedLastBallPos.x > rightBarPos.x ) {
            // collision with the top of the bar
            ballPos.y = rightBarPos.y;
            ballVelocity.y *= -1;
        }
        else if (modifiedLastBallPos.y > rightBarPos.y - barDims.y && modifiedLastBallPos.x < rightBarPos.x ) {
            // collision with the side of the bar
            ballPos.x = rightBarPos.x - ballDims.x;

            // we adjust the y velocity based on where on the paddle the ball collides
            float hitDist = (rightBarPos.y - barDims.y / 2) - ballPos.y;
            // getting a float between 0 and 1 and negating since we are on the opposite side
            hitDist /=  - barDims.y / 2;
            ballVelocity.y = ballVelocity.x * hitDist;

            ballVelocity.x *= -1;

            
        }
        else if (modifiedLastBallPos.y < rightBarPos.y - barDims.y && modifiedLastBallPos.x > rightBarPos.x){
            // collision with the bottom of the bar
            ballPos.y = rightBarPos.y - barDims.y;
            ballVelocity.y *= -1;
        }
        else if (modifiedLastBallPos.y > rightBarPos.y && modifiedLastBallPos.x < rightBarPos.x) {
            // top left corner collision
            ballPos.y = rightBarPos.y;
            ballPos.x = rightBarPos.x - ballDims.x;
            ballVelocity *= -1;
        }
        else if (modifiedLastBallPos.y < rightBarPos.y - barDims.y && modifiedLastBallPos.x < rightBarPos.x) {
            // bottom left corner collision
            ballPos.x = rightBarPos.x - ballDims.x;
            ballPos.y = rightBarPos.y - barDims.y;
            ballVelocity *= -1;
        }
        
    } 
    
    return 0;
}

void PongState::handleAIMovement()
{
    // we just need to move the bar up or down just the right amount to hit the ball based on its trajectory
    // this AI is simple, it will not take into account the complex future (ball bouncing off walls etc.)
    // this AI should not have access to the balls velocity vector, but it can "observe" changes in position of the ball    
    glm::vec2 ballTrajectory = ballPos - ballLastPos;

    // how many timesteps will it take for the ball approximately to reach the right bar?
    float remainingDistance = rightBarPos.x - (ballPos.x + ballDims.x);
    float numTimesteps = remainingDistance / (ballTrajectory.x / timeDelta);
    
    // will we be in a good position for the bar to collide with the ball?
    float estimatedY = ballPos.y + (ballTrajectory.y * numTimesteps);

    if (estimatedY > rightBarPos.y) {
        // we move up
        rightBarPos.y = std::min(1.0f, rightBarPos.y + timeDelta * barSpeedMultiplier);
    }
    else if (estimatedY < rightBarPos.y - barDims.y) {
        // we move down
        rightBarPos.y = std::max(-1.0f+barDims.y, rightBarPos.y - timeDelta * barSpeedMultiplier);
    }


}

void PongState::resetGame(bool totalReset) {
    // initializing positions as the top left vertex of each object with even distances
    leftBarPos = glm::vec2(-0.95f,0.2f);
    rightBarPos = glm::vec2(0.91f,0.2f);
    ballPos = glm::vec2(-0.02f,0.02f);
    ballLastPos = glm::vec2(-0.02f, 0.02f);

    if (totalReset) {
        // reset the score also
        leftScore = 0;
        rightScore = 0;
    }
    
    // reset ball velocity
    setBallInitialDirection();

}

void PongState::handleMovement(GLFWwindow* window)
{
    int state = glfwGetKey(window, GLFW_KEY_UP);
    if (state != GLFW_RELEASE) {
        // move the left bar up, we should not move it above the top of the screen!
        leftBarPos.y = std::min(1.0f, leftBarPos.y + timeDelta * barSpeedMultiplier);

        // for debuggging without AI we will move the right bar also
        //rightBarPos.y = std::min(1.0f, rightBarPos.y + timeDelta * barSpeedMultiplier);
        return;
    }

    state = glfwGetKey(window, GLFW_KEY_DOWN);
    if (state != GLFW_RELEASE) {
        // move the left bar down and the bottom of the bar should not go below the screen!
        leftBarPos.y = std::max(-1.0f+barDims.y, leftBarPos.y - timeDelta * barSpeedMultiplier); 

        // for debugging without AI we will move the right bar also
        //rightBarPos.y = std::max(-1.0f+barDims.y, rightBarPos.y - timeDelta * barSpeedMultiplier); 
    }
}

void PongState::setTimeDelta(float timeDelta)
{
    this->timeDelta = timeDelta;
}

void PongState::setBallInitialDirection()
{
    ballVelocity.x = ballSpeedMultiplier;
    ballVelocity.y = ballSpeedMultiplier * glm::cos(sampleRandom());

    // determining sign randomly	
    if (sampleRandom() > 0.5) {
        ballVelocity.x *= -1;
    }
    if (sampleRandom() > 0.5) {
        ballVelocity.y *= -1;
    }
    
    //printf("init ball velocity: %f ----- %f \n", ballVelocity.x, ballVelocity.y);
}




