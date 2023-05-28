#include "../Includes/Pong.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ctime>
// Pong.cpp holds logic for building and running the pong game (AI, vertices, etc.)

PongState::PongState()
{
    // seeding the random number generation
    srand(time(0));
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

    // randomize ball velocity vector direction and normalize to set the initial direction
    ballVelocity = glm::vec2(0.0f, 0.0f);
	ballVelocity.x = rand()/INT_MAX+1;
	ballVelocity.y = rand() / INT_MAX + 1;

	// setting default values in case of zeros
	if (ballVelocity.x == 0.0f) ballVelocity.x = 0.02;
	if (ballVelocity.y == 0.0f) ballVelocity.y = 0.02;

    // normalizing
    ballVelocity = glm::normalize(ballVelocity);

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
    /*
	float* vertices = new float[24]{
		-0.95f,-0.20f,0.00f,1.0f,0.0f,0.0f, // bottom left of rectangle
		 -0.91f,-0.20f, 0.00f, 1.0f, 0.0f, 0.0f,// bottom right of rectangle
		-0.95f,0.20f,0.00f,1.0f, 0.0f, 0.0f, // top left of rectangle
		 -0.91f,0.20f,0.00f,1.0f, 0.0f, 0.0f // top right of rectangle

	};
    */
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
    /*
	float* vertices = new float[24]{
		-0.02f,-0.02f,0.00f,1.0f,0.0f,0.0f, // bottom left of rectangle
		 0.02f,-0.02f, 0.00f, 1.0f, 0.0f, 0.0f,// bottom right of rectangle
		-0.02f,0.02f,0.00f,1.0f, 0.0f, 0.0f, // top left of rectangle
		 0.02f,0.02f,0.00f,1.0f, 0.0f, 0.0f // top right of rectangle

	};
    */
	return vertices;
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
    ballPos.x += ballVelocity.x * timeDelta * ballSpeedMultiplier;
    ballPos.y += ballVelocity.y * timeDelta * ballSpeedMultiplier;

    // check for collisions and reset ball if necessary (after scoring)
}

void PongState::handleMovement(GLFWwindow* window)
{
    int state = glfwGetKey(window, GLFW_KEY_UP);
    if (state != GLFW_RELEASE) {
        // move the left bar up, we should not move it above the top of the screen!
        leftBarPos.y = std::min(1.0f, leftBarPos.y + timeDelta * barSpeedMultiplier);
        return;
    }

    state = glfwGetKey(window, GLFW_KEY_DOWN);
    if (state != GLFW_RELEASE) {
        // move the left bar down and the bottom of the bar should not go below the screen!
        leftBarPos.y = std::max(-1.0f+barDims.y, leftBarPos.y - timeDelta * barSpeedMultiplier); 
    }
}

void PongState::setTimeDelta(float timeDelta)
{
    this->timeDelta = timeDelta;
}


