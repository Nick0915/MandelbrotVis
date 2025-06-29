cmake_minimum_required(VERSION 3.5.0)
project(mandelbrot VERSION 0.1.0)

# add_compile_options(-g -Og)
add_compile_options(-O3)

include_directories(include/ shader/)

aux_source_directory(src SRC_FILES)

add_executable(mandelbrot ${SRC_FILES})

target_link_libraries(mandelbrot glfw m)
