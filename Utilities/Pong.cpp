#include "../Includes/Pong.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ctime>
#include <random>
// Pong.cpp holds logic for building and running the pong game (AI, vertices, etc.)

PongState::PongState()
{
    // we start with default settings in the state
    ballSpeedMultiplier = 1;
    barSpeedMultiplier = 5;

    // setting up shaders to use with our pong state
    leftBarShader = new Shader("Vertex_Shaders/color_shader.vs", "Fragment_Shaders/color_shader.fs");
    rightBarShader = new Shader("Vertex_Shaders/color_shader.vs", "Fragment_Shaders/color_shader.fs");
    ballShader = new Shader("Vertex_Shaders/color_shader.vs", "Fragment_Shaders/color_shader.fs");    

	// generating vertices
	leftBarLength = 24, rightBarLength = 24, ballLength = 24;
	leftBarVertices = generateBar();
	rightBarVertices = generateBar();
	ballVertices = generateBall();

    barDims = glm::vec2(leftBarVertices[6]-leftBarVertices[0],leftBarVertices[2*6+1]-leftBarVertices[1]);
    ballDims = glm::vec2(ballVertices[6]-ballVertices[0],ballVertices[2*6+1]-ballVertices[1]);

    // initializing positions as the top left vertex of each object with even distances
    leftBarPos = glm::vec2(-0.95f,0.2f);
    rightBarPos = glm::vec2(0.91f,0.2f);
    ballPos = glm::vec2(-0.02f,0.02f);
    ballLastPos = glm::vec2(-0.02f, 0.02f);

    // initializing the random distribution to sample from
    setupDistribution();

    // randomize ball velocity vector direction and normalize to set the initial direction
    setBallInitialDirection();

	// indices to draw rectangles from triangle coordinates
	indices = generateIndices();
	indicesLength = 6;

	// opengl setup for all our VAOs for all our objects
    glGenVertexArrays(1, &leftBarVAO);
    glGenVertexArrays(1, &rightBarVAO);
    glGenVertexArrays(1, &ballVAO);

    // setting up the left bar first
    glBindVertexArray(leftBarVAO);
    
    // initialize VBO ids
    glGenBuffers(1, &leftBarVBO);
    glGenBuffers(1, &rightBarVBO);
    glGenBuffers(1, &ballVBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, leftBarVBO);
    glBufferData(GL_ARRAY_BUFFER, leftBarLength * sizeof(float), leftBarVertices, GL_DYNAMIC_DRAW);

    // initialize our EBO ids (we are drawing rectangles so lets draw by indices in the EBO)
    glGenBuffers(1, &leftBarEBO);
    glGenBuffers(1, &rightBarEBO);
    glGenBuffers(1, &ballEBO);
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

}

void PongState::draw(GLFWwindow* window)
{
    // handling movement
    handleMovement(window);
    handleAIMovement();
    handleBallMovement();

    leftBarShader->use();
    // sending uniform for translation
    glm::mat4 translation = glm::mat4(1.0f);
    translation = glm::translate(translation, glm::vec3(leftBarPos, 0.0f));
    int transformLoc = glGetUniformLocation(leftBarShader->ID, "transformation");
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
*/
float* PongState::generateScoreBoard()
{
    //TODO 
}

unsigned int* PongState::generateIndices()
{
	unsigned int* indices = new unsigned int[6] {
		0,1,2, // first triangle
		2,3,1  // second triange
	};
	return indices;
}

void PongState::handleBallMovement()
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
        resetGame();
        return;
    }
    else if (ballPos.x + ballDims.x >= 1.0f) {
        // goal state! this is a goal for the left player (hit the right wall)
        resetGame();
        return;
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

void PongState::resetGame() {
    // initializing positions as the top left vertex of each object with even distances
    leftBarPos = glm::vec2(-0.95f,0.2f);
    rightBarPos = glm::vec2(0.91f,0.2f);
    ballPos = glm::vec2(-0.02f,0.02f);
    ballLastPos = glm::vec2(-0.02f, 0.02f);
    
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


