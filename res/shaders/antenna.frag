#version 410

in float wave_power;

uniform float time;

out vec4 out_wave;

void main() {
    out_wave = vec4(wave_power * sin(time), 0.0F, 0.0F, 1.0F);

    // tmp
    out_wave.xyz = out_wave.xyz * 0.5F + 0.5F;
}
