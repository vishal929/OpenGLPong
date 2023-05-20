#include <iostream>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Includes/Shader.hpp"
#include "Includes/glHelpers.hpp"



int main()
{
    if (!glfwInit()){
        printf("failed to initialize glfw context!\n");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    // for mac osx glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    

    // simple shaders initialization
    Shader shader("./Vertex_Shaders/standard_shader.vs", "./Fragment_Shaders/standard_shader.fs");
    
    // our shape of a triangle
    float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
    };

    // declaring a VAO (so we can keep our state for drawing these vertices in an object and easily swap between states)
    // this example uses a single state, but I am just keeping this for learning
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // creating a buffer
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // declaring that our buffer is a vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // cpu to gpu transfer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // describing the buffer to opengl
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // unbinding VAO 
    glBindVertexArray(0);
    
    glDrawArrays(GL_TRIANGLES, 0, 3);

    //render loop
    while(!glfwWindowShouldClose(window)){
        //testing a color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        shader.use();
        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }
    
    // cleanup
    glfwTerminate();
    return 0;
}

  