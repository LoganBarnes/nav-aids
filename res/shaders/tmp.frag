#version 450

in vec2 clip_position;

out vec4 out_color;

void main() {
    out_color = vec4(clip_position, 1.0F, 1.0F);
}
