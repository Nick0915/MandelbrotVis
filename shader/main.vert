#version 460 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 uv_in;

layout (location = 0) out vec2 uv_out;

void main() {
    gl_Position = vec4(pos.x, pos.y, 0., 1.);

    uv_out = uv_in;
}
