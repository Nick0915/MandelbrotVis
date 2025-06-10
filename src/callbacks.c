#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <callbacks.h>

void resize_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
}

// void cursor_pos_callback(GLFWwindow* window, double x, double y);
// void scroll_callback(GLFWwindow* window, double x, double y);
