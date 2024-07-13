#version 410

// https://beltoforion.de/en/recreational_mathematics/2d-wave-equation.php

in vec2 in_clip_position;

out vec2 clip_position;

void main() {
    clip_position = in_clip_position;
    gl_Position = vec4(in_clip_position, 1.0F, 1.0F);
}
