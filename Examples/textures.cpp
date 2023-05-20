#include <iostream>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Includes/Shader.hpp"
#include "Includes/glHelpers.hpp"
#include "Includes/stb_image.h"



int texture_example()
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
    Shader shader("./Vertex_Shaders/texture_shader.vs", "./Fragment_Shaders/double_texture_shader.fs");

    // texture options
    stbi_set_flip_vertically_on_load(true);

    unsigned int texture,texture2;
    glGenTextures(1, &texture);
    glGenTextures(1, &texture2);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // loading our texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("Textures/container.jpg", &width, &height, &nrChannels, 0);

    if (data == nullptr) {
        // we failed to load the texture
        std::cout << "FAILED::LOADING::TEXTURE!" << std::endl;
        return 1;
    }

    // generating texture and mipmap based on loaded image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // mipmaps are already generated, we can safely free the memory of the image
    stbi_image_free(data);

    //second texture options
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // adding a second texture for fun!
    data = stbi_load("Textures/awesomeface.png", &width, &height, &nrChannels, 0);

    if (data == nullptr) {
        // we failed to load the texture
        std::cout << "FAILED::LOADING::TEXTURE!" << std::endl;
        return 1;
    }

    // generating texture and mipmap based on loaded image (THIS IMAGE INCLUDES AN ALPHA CHANNEL!)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // mipmaps are already generated, we can safely free the memory of the image
    stbi_image_free(data);

    
    // our vertices for the container
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    

    // declaring a VAO (so we can keep our state for drawing these vertices in an object and easily swap between states)
    // this example uses a single state, but I am just keeping this for learning
    unsigned int VAO, VBO, EBO;

    // creating vertex array object (state)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // creating a buffer
    glGenBuffers(1, &VBO);

    // creating the element buffer object (to reference vertices by index)
    glGenBuffers(1, &EBO);

    // declaring that our buffer is a vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // cpu to gpu transfer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // handling EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // describing the buffer to opengl

    //vertex attribute (index 0, number components 3, stride is 8* sizeof(float), offset is 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // clearing currently bound buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // unbinding VAO 
    glBindVertexArray(0);
    
    // setting uniforms
    shader.use();
    shader.setFloat("interpolation_const", 0.6);
    shader.setInt("firstTexture", 0);
    shader.setInt("secondTexture", 1);

    //render loop
    while(!glfwWindowShouldClose(window)){
        //testing a color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        shader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }
    
    // cleanup
    glfwTerminate();
    return 0;
}
