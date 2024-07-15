#version 410

// https://beltoforion.de/en/recreational_mathematics/2d-wave-equation.php

uniform float spatial_step = 1.0F;
uniform float time_step    = 1.0F;
uniform float speed        = 0.5F;

uniform vec2      state_size;
uniform sampler2D prev_state;
uniform sampler2D curr_state;

out vec4 next_value;

vec2 tex_coords(in vec2 offset)
{
    return (gl_FragCoord.xy + offset) / state_size;
}

void main()
{
    float alpha = pow((speed * time_step) / spatial_step, 2.0F);

    vec4 prev_value = texture(prev_state, tex_coords(vec2(0.0F, 0.0F)));
    vec4 curr_value = texture(curr_state, tex_coords(vec2(0.0F, 0.0F)));

    next_value = texture(curr_state, tex_coords(vec2(-1.0F, 0.0F)))
    + texture(curr_state, tex_coords(vec2(+1.0F, 0.0F)))
    + texture(curr_state, tex_coords(vec2(0.0F, -1.0F)))
    + texture(curr_state, tex_coords(vec2(0.0F, +1.0F)))
    - 4.0F * curr_value;

    next_value *= alpha;
    next_value += 2.0F * curr_value - prev_value;

//    next_value = curr_value;
}
