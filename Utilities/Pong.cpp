#include "../Includes/Pong.hpp"
// Pong.cpp holds logic for building and running the pong game (AI, vertices, etc.)

PongState::PongState()
{
    // we start with default settings in the state
    ballSpeedMultiplier = 1;
    barSpeedMultiplier = 1;

    // setting up shader to use with our pong state
    shader = new Shader("Vertex_Shaders/color_shader.vs", "Fragment_Shaders/color_shader.fs");

	// generating vertices
	leftBarLength = 24, rightBarLength = 24, ballLength = 24;
	leftBarVertices = generateLeftBar();
	rightBarVertices = generateRightBar();
	ballVertices = generateBall();

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

void PongState::draw()
{
    shader->use();
	glBindVertexArray(leftBarVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(rightBarVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
    delete(shader);
}

/*
	vertices in 2d representing the left player's bar in pong in normalized coordinates
	We return 6d coordinates where the z dimension is 0 and the last 3 dimensions is the color
*/
float* PongState::generateLeftBar()
{
	float* vertices = new float[24]{
		-0.95f,-0.20f,0.00f,1.0f,0.0f,0.0f, // bottom left of rectangle
		 -0.91f,-0.20f, 0.00f, 1.0f, 0.0f, 0.0f,// bottom right of rectangle
		-0.95f,0.20f,0.00f,1.0f, 0.0f, 0.0f, // top left of rectangle
		 -0.91f,0.20f,0.00f,1.0f, 0.0f, 0.0f // top right of rectangle

	};
	return vertices;
}

/*
	vertices in 2d representing the right player's bar in pong in normalized coordinates
	We return 4d coordinates where the z dimension is 0 and the last dimension is the color
*/
float* PongState::generateRightBar()
{
	float* vertices = new float[24]{
		0.95f,-0.20f,0.00f,1.0f,0.0f,0.0f, // bottom left of rectangle
		 0.91f,-0.20f, 0.00f, 1.0f, 0.0f, 0.0f,// bottom right of rectangle
		0.95f,0.20f,0.00f,1.0f, 0.0f, 0.0f, // top left of rectangle
		 0.91f,0.20f,0.00f,1.0f, 0.0f, 0.0f // top right of rectangle

	};
	return vertices;
}

/*
	vertices in 2d presenting the ball in pong in normalized coordinates
	We return 4d coordinates where the z dimension is 0 and the last dimension is the color
*/
float* PongState::generateBall()
{
	float* vertices = new float[24]{
		-0.02f,-0.02f,0.00f,1.0f,0.0f,0.0f, // bottom left of rectangle
		 0.02f,-0.02f, 0.00f, 1.0f, 0.0f, 0.0f,// bottom right of rectangle
		-0.02f,0.02f,0.00f,1.0f, 0.0f, 0.0f, // top left of rectangle
		 0.02f,0.02f,0.00f,1.0f, 0.0f, 0.0f // top right of rectangle

	};
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
