#version 410

out vec2 texture_coordinates;

uniform sampler2D wave_texture;

out vec4 out_color;

void main() {
    vec4 wave_value = texture(wave_texture, texture_coordinates);
    wave_value      = wave_value * 0.5F + 0.5F;

    out_color = vec4(wave_value.r, 0.5F, 0.5F, 1.0F);
}
