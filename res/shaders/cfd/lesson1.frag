#version 410 core

uniform sampler2D prev_state;
uniform vec2      framebuffer_size = vec2(1, 1);

out vec4 out_color;

void main()
{
    vec2 texture_coordinates = gl_FragCoord.xy / framebuffer_size;
    vec4 value          = texture(prev_state, texture_coordinates);

    out_color = value;
}
