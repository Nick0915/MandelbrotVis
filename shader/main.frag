#version 460 core

// uniform float window_width, window_height;
uniform vec2 u_resolution;
uniform vec2 u_pan;
uniform float u_zoom;
// uniform float x_off, y_off;

out vec3 color;

vec2 screen2ndc(vec2 screen_coords) {
    return (screen_coords / u_resolution - 0.5f) * 2.f;
}

vec2 ndc2screen(vec2 ndc_coords) {
    return ((ndc_coords / 2.f) + .5f) * u_resolution;
}

void main() {
    // vec2 screen_coords = ((gl_FragCoord.xy) - u_resolution / 2.f) * u_zoom + (u_resolution / 2.f);

    // float x = ((gl_FragCoord.x + x_off) - (window_width / 2.f)) * zoom + (window_width / 2.f);
    // float y = ((gl_FragCoord.y + y_off) - (window_height / 2.f)) * zoom + (window_height / 2.f);

    // float x = (gl_FragCoord.x - (window_width / 2.f)) * zoom;
    // float y = (gl_FragCoord.y - (window_height / 2.f)) * zoom;

    vec2 xy = (screen2ndc(gl_FragCoord.xy) * u_zoom) + screen2ndc(u_pan);
    vec2 colorxy = (xy + 1.f) / 2.f;

    if (xy.x < -1.f || xy.y < -1.f || xy.x >= 1.f || xy.y >= 1.f) {
        color = vec3(0.f, 0.f, 0.f);
    } else {
        color = vec3(colorxy, 1.f);
    }
}