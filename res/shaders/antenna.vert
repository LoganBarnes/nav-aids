#version 410

in vec2  world_position;
in float antenna_power;

uniform mat4 projection_from_world = mat4(1.0F);

out float wave_power;

void main() {
    wave_power  = antenna_power;
    gl_Position = projection_from_world * vec4(world_position, 0.0F, 1.0F);
}
