#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <inttypes.h>
#include <float.h>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <callbacks.h>

float window_width = 800.f;
float window_height = 600.f;
float zoom = 1.f;
float x_off = 0.f;
float y_off = 0.f;

uint32_t create_shader_program();
GLFWwindow* init_window();
void process_input(GLFWwindow* window);
void cleanup(GLFWwindow* window, uint32_t shader_program);
bool read_file(const char* filename, unsigned char **out_buffer, size_t* out_length);

int32_t main() {
    GLFWwindow* window = init_window();

    // static float vertices[] = {
    //     -1.f,  1.f, // 0: top-left pos
    //     -1.f, -1.f, // 1: bottom-left pos
    //      1.f, -1.f, // 2: bottom-right pos
    //      1.f,  1.f, // 3: top-right pos
    //     -1.f,  1.f, // 4: top-left pos
    // };

    static float vertices[] = {
        -1.f,  1.f, -1.f,  1.f, // 0: top-left pos, uv
        -1.f, -1.f, -1.f, -1.f, // 1: bottom-left pos, uv
         1.f, -1.f,  1.f, -1.f, // 2: bottom-right pos, uv
         1.f,  1.f,  1.f,  1.f, // 3: top-right pos, uv
    };

    static uint32_t indices[] = {
        0, 1, 2,    // bottom-left tri
        2, 3, 0,    // top-right tri
    };

    uint32_t VAO, VBO, EBO;

    // VAO: the bound VAO stores the preferred "settings" of a draw call
    // (glEnableVertexAttribArray, glVertexAttribPointer)
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
    }

    // EBO: stores indices to use in drawing mesh (to avoid repeating shared
    // vertices in VBO)
    {
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    }

    // VBO: stores the actual vertex data
    {
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(
            0,                      // layout (location = 0) in
            2,                      // vec2 pos;
            GL_FLOAT,               // data type
            GL_FALSE,               // normalization
            4 * sizeof(float),      // stride between each position
            (void*) 0               // where in the buffer the data starts (at the beginning, here)
        );
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(
            1,                      // layout (location = 1) in
            2,                      // vec2 uv;
            GL_FLOAT,               // data type
            GL_FALSE,               // normalization
            4 * sizeof(float),      // stride between each uv
            (void*) &(vertices[2])  // where in buffer the data starts (at the second element, here)
        );
        glEnableVertexAttribArray(1);
    }

    uint32_t shader_program;
    uint32_t uni_loc_window_width, uni_loc_window_height, uni_loc_zoom, uni_loc_x_off, uni_loc_y_off;

    // shader
    {
        shader_program = create_shader_program();
        glUseProgram(shader_program);

        uni_loc_window_width = glGetUniformLocation(shader_program, "window_width");
        uni_loc_window_height = glGetUniformLocation(shader_program, "window_height");
        uni_loc_zoom = glGetUniformLocation(shader_program, "zoom");
        uni_loc_x_off = glGetUniformLocation(shader_program, "x_off");
        uni_loc_y_off = glGetUniformLocation(shader_program, "y_off");
    }

    uint64_t frame_no = 0;
    double start_time = glfwGetTime();
    double last_frame_time = start_time;
    double report_timer = 1.f;
    uint64_t num_frames_since_report = 0;

    glClearColor(0.1f, 0.1f, 0.15f, 0.f);
    while (!glfwWindowShouldClose(window)) {
        process_input(window);

        // update uniformss
        {
            glUniform1f(uni_loc_window_width, (float) window_width);
            glUniform1f(uni_loc_window_height, (float) window_height);
            glUniform1f(uni_loc_zoom, (float) zoom);
            glUniform1f(uni_loc_x_off, (float) x_off);
            glUniform1f(uni_loc_y_off, (float) y_off);
        }

        // render
        {

            glClear(GL_COLOR_BUFFER_BIT);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            // glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        frame_no++;
        num_frames_since_report++;

        double current_time = glfwGetTime();
        double delay = current_time - last_frame_time;
        report_timer -= delay;

        if (report_timer < 0) {
            double actual_time = 1.f - report_timer;
            double fps = (double) num_frames_since_report / actual_time;
            char* title;
            asprintf(&title, "Game of Life (%.2f fps)", fps);
            glfwSetWindowTitle(window, title);
            free(title);

            num_frames_since_report = 0;
            report_timer = 1.f;
        }

        // if (frame_no % report_every == 0) {
        //     char* title;
        //     asprintf(&title, "Game of Life (%.2f fps)", 1.f / delay);
        //     glfwSetWindowTitle(window, title);
        //     free(title);
        // }

        last_frame_time = current_time;
        glfwPollEvents();
    }

    cleanup(window, shader_program);
}

uint32_t create_shader_program() {
    char vert_source[1024];
    char frag_source[1024];

    size_t trash = 0;

    if (!read_file("shader/main.vert", (unsigned char**)&vert_source, &trash)) {
        fprintf(stderr, "Failed to read shader/main.vert\n");
    }

    if (!read_file("shader/main.frag", (unsigned char**)&frag_source, &trash)) {
        fprintf(stderr, "Failed to read shader/main.frag\n");
    }

    int success;
    int failed = 0;
    char shader_log[1024];

    uint32_t vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, (const char* const*)&vert_source, NULL);
    glCompileShader(vert_shader);
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vert_shader, 1024, NULL, shader_log);
        fprintf(stderr, "Vertex shader compilation failed:\n%s\n", shader_log);
        failed = 1;
    }

    uint32_t frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, (const char* const*)&frag_source, NULL);
    glCompileShader(frag_shader);
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(frag_shader, 1024, NULL, shader_log);
        fprintf(stderr, "Vertex shader compilation failed:\n%s\n", shader_log);
        failed = 1;
    }

    uint32_t shader_program = glCreateProgram();
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
        window_width, window_height,
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

    glfwSwapInterval(0);

    glfwSetFramebufferSizeCallback(window, resize_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);

    printf("Window created\n");
    return window;
}

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void cleanup(GLFWwindow* window, uint32_t shader_program) {
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
