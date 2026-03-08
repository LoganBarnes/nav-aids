#version 410

in vec2  world_position;
in float antenna_power;
in float phase_rads;

uniform mat4 clip_from_world = mat4(1.0F);

out float wave_power;
out float phase;

void main() {
    wave_power   = antenna_power;
    phase        = phase_rads;
    gl_Position  = clip_from_world * vec4(world_position, 0.0F, 1.0F);
    gl_PointSize = 5.0F;
}
