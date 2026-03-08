#version 410

in float wave_power;
in float phase;

uniform float time_s       = 0.0F;
uniform float frequency_hz = 10.0F;

out vec4 out_wave;

void main() {
    if (length(gl_PointCoord.xy) > 1.0F)
    {
        discard;
    }
    out_wave = vec4(wave_power * sin(time_s * frequency_hz + phase), 0.0F, 0.0F, 0.0F);
}
