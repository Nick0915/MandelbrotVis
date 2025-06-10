cmake_minimum_required(VERSION 3.5.0)
project(gol VERSION 0.1.0)

add_compile_options(-g -Og)

include_directories(include/ shader/)

aux_source_directory(src SRC_FILES)

add_executable(gol ${SRC_FILES})

target_link_libraries(gol glfw)
