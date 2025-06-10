#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <callbacks.h>

unsigned int create_shader_program();
void render(unsigned int EBO);
GLFWwindow* init_window();
void process_input(GLFWwindow* window);
void cleanup(GLFWwindow* window, unsigned int shader_program);
bool read_file(const char* filename, unsigned char **out_buffer, size_t* out_length);

int main() {
    GLFWwindow* window = init_window();

    static float vertices[] = {
        -1.f,  1.f, // 0: top-left pos
         0.f,  1.f, // 0: top-left uv
         1.f,  1.f, // 1: top-right pos
         1.f,  1.f, // 1: top-right uv
         1.f, -1.f, // 2: bottom-right pos
         1.f,  0.f, // 2: bottom-right uv
        -1.f, -1.f, // 3: bottom-left pos
         0.f,  0.f, // 3: bottom-left uv
    };

    static int indices[] = {
        0, 1, 2,
        2, 3, 0,
    };

    unsigned int VBO, EBO, VAO;

    // VAO
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
    }

    // VBO
    {
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(
            0,                      // layout (location = 0) in
            2,                      // vec2 pos;
            GL_FLOAT,               // data type
            GL_FALSE,               // normalization
            4 * sizeof(float),      // stride between new vertices
            (void*) 0               // where in the buffer the data starts (at the beginning, here)
        );
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(
            1,                      // layout (location = 1) in
            2,                      // vec2 uv;
            GL_FLOAT,               // data type
            GL_FALSE,               // normalization
            4 * sizeof(float),      // stride between new vertices
            (void*) 0               // where in the buffer the data starts (at the beginning, here)
        );
        glEnableVertexAttribArray(1);
    }

    // EBO
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    }

    unsigned int shader_program = create_shader_program();
    glUseProgram(shader_program);

    glClearColor(0.1f, 0.1f, 0.15f, 0.f);
    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        render(EBO);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanup(window, shader_program);
}

void render(unsigned int EBO) {
    glClear(GL_COLOR_BUFFER_BIT);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, 0);
}

unsigned int create_shader_program() {
    char vert_source[1024];
    char frag_source[1024];

    size_t trash = 0;

    if (!read_file("shader/main.vert", &vert_source, &trash)) {
        fprintf(stderr, "Failed to read shader/main.vert\n");
    }

    if (!read_file("shader/main.frag", &frag_source, &trash)) {
        fprintf(stderr, "Failed to read shader/main.frag\n");
    }

    int success;
    int failed = 0;
    char shader_log[1024];

    unsigned int vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, &vert_source, NULL);
    glCompileShader(vert_shader);
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vert_shader, 1024, NULL, shader_log);
        fprintf(stderr, "Vertex shader compilation failed:\n%s\n", shader_log);
        failed = 1;
    }

    unsigned int frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, &frag_source, NULL);
    glCompileShader(frag_shader);
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag_shader, 1024, NULL, shader_log);
        fprintf(stderr, "Vertex shader compilation failed:\n%s\n", shader_log);
        failed = 1;
    }

    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vert_shader);
    glAttachShader(shader_program, frag_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 1024, NULL, shader_log);
        fprintf(stderr, "Shader program linking failed:\n%s\n", shader_log);
        failed = 1;
    }

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    if (failed) {
        fprintf(stderr, "Exiting...\n");
        exit(-1);
    }

    return shader_program;
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

void cleanup(GLFWwindow* window, unsigned int shader_program) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool read_file(const char *filename, unsigned char **out_buffer, size_t *out_length) {
#   define CHUNK_SIZE (1024*1024)
    // Open file
    FILE *f = fopen(filename, "rb");
    if(!f)
        return false;

    // Initialize output
    *out_buffer = NULL;
    *out_length = 0;

    while(!feof(f)) {
        // Expand allocation
        {
            void *new_mem = realloc(*out_buffer, *out_length + CHUNK_SIZE);
            if(new_mem == NULL)
                goto error;
            *out_buffer = new_mem;
        }

        // Read data
        size_t bytes_read = fread(&(*out_buffer)[*out_length], 1, CHUNK_SIZE, f);
        if(ferror(f))
            goto error;
        *out_length += bytes_read;

        // Shrink allocation if less than CHUNK_SIZE bytes were read
        if(bytes_read < CHUNK_SIZE) {
            void *new_mem = realloc(*out_buffer, *out_length);
            if(new_mem == NULL)
                goto error;
            *out_buffer = new_mem;
        }
    }

    fclose(f);
    return true;

error:
    fclose(f);
    free(*out_buffer);
    return false;
#   undef CHUNK_SIZE
}
