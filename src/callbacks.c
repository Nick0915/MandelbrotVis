#include <inttypes.h>
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <callbacks.h>

typedef uint8_t bool;

#define ZOOM_AMT ((float) 0.1f)
#define true ((bool) 0xFF)
#define false ((bool) 0x00)

extern float window_width;
extern float window_height;
extern float zoom;
extern float x_off, y_off;

float mouse_x, mouse_y;
float drag_prev_x = -1, drag_prev_y = -1;
uint8_t is_dragging = false;

void resize_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    window_width = width;
    window_height = height;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    return;
}

void scroll_callback(GLFWwindow* window, double x, double y) {
    // printf("scrolled %.3lf\n", y);
    zoom -= ZOOM_AMT * (float) y;
    if (zoom <= 0.1f) {
        zoom = 0.1f;
    }
}

void cursor_pos_callback(GLFWwindow* window, double x, double y) {
    mouse_x = (float) x;
    mouse_y = (float) y;

    if (is_dragging) {
        float x_drag = mouse_x - drag_prev_x;
        float y_drag = mouse_y - drag_prev_y;

        x_off -= x_drag; // / window_width;
        y_off += y_drag; // / window_height;

        drag_prev_x = mouse_x;
        drag_prev_y = mouse_y;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    // TODO
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        is_dragging = true;
        drag_prev_x = mouse_x;
        drag_prev_y = mouse_y;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        is_dragging = false;
        drag_prev_x = -1.f;
        drag_prev_y = -1.f;
    }
}
