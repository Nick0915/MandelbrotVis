#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <callbacks.h>

GLFWwindow* init_window();
void cleanup(GLFWwindow* window);
void process_input(GLFWwindow* window);

int main() {
    GLFWwindow* window = init_window();

    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanup(window);
}

GLFWwindow* init_window() {
    if (!glfwInit()) {
        fprintf(stderr, "Couldn't initialize GLFW!\n");
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        800, 600,
        "Game of Life",
        NULL, NULL
    );
    if (!window) {
        fprintf(stderr, "Couldn't create window!\n");
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Couldn't load GLAD!\n");
        exit(-1);
    }
    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, resize_callback);
    glfwSetKeyCallback(window, key_callback);

    printf("Window created\n");
    return window;
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void cleanup(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}
