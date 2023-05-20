// helper functions for opengl we might need
#include "../Includes/glHelpers.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/*
    Function to adjust the window of opengl as a callback
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

