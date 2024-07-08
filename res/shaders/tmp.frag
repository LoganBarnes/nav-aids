#version 410

in vec2 clip_position;

out vec4 out_color;

void main() {
    out_color = vec4(clip_position * 0.5F + 0.5F, 1.0F, 1.0F);
}
