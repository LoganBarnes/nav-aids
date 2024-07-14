#version 410

// https://beltoforion.de/en/recreational_mathematics/2d-wave-equation.php

in vec2 texture_coordinates;

uniform sampler2D previous_state;

out vec4 current_value;

void main() {
    vec4 previous_value = texture(previous_state, texture_coordinates);

    current_value = previous_value;
}
