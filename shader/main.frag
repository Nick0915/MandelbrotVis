#version 460 core

uniform float window_width, window_height;
uniform float zoom;
uniform float x_off, y_off;

out vec3 color;

void main() {
    float x = ((gl_FragCoord.x + x_off) - (window_width / 2.f)) * zoom + (window_width / 2.f);
    float y = ((gl_FragCoord.y + y_off) - (window_height / 2.f)) * zoom + (window_height / 2.f);

    // float x = (gl_FragCoord.x - (window_width / 2.f)) * zoom;
    // float y = (gl_FragCoord.y - (window_height / 2.f)) * zoom;

    if (x < 0 || y < 0 || x >= window_width || y >= window_height) {
        color = vec3(0.f, 0.f, 0.f);
    } else {
        color = vec3(x / window_width, y / window_height, 1.f);
    }
}