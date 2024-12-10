#version 410 core

uniform sampler2D wave_texture;
uniform vec2      framebuffer_size = vec2(1, 1);

out vec4 out_color;

void main()
{
    vec2 texture_coordinates = gl_FragCoord.xy / framebuffer_size;
    vec4 wave_value          = texture(wave_texture, texture_coordinates);

    out_color = wave_value;
}
