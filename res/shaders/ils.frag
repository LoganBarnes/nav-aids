#version 410

const float pi               = 3.14159265359F;
const float two_pi_over_c_km = 2.09584502e-5F;// s/km
const float loc_freq         = 109100.0F;// kHz
const float one_fifty_freq   = 0.15F;// kHz
const float ninety_freq      = 0.09F;// kHz

uniform float pixel_size_m = 1.0F;// m

uniform int   antenna_pairs = 1;
uniform float antenna_spacing_m = 5.0F;//m

uniform vec3 output_scale = vec3(0.1F, 0.0F, 0.0F);
//uniform vec3 output_scale = vec3(0.0F, 0.1F, 0.0F);
//uniform vec3 output_scale = vec3(0.0F, 0.0F, 0.1F);

//uniform float time_s;
uniform vec2  field_size_pixels;

out vec4 frag_color;

vec2 value(in vec2 position, in vec2 antenna, in float frequency, in float phase) {
    float dist = length(position - antenna);
    float x    = dist * two_pi_over_c_km * frequency + phase;
    return vec2(cos(x), sin(x));
}

void main()
{
    vec2 pixel_pos = gl_FragCoord.xy - vec2(0.0F, field_size_pixels.y / 2.0F);
    vec2 position  = pixel_pos * pixel_size_m;

    vec3 signal = vec3(0.0F, 0.0F, 0.0F);

    //                 carrier, 90hz, 150hz
    vec3 ninety    = vec3(1.0F, 0.5F, 0.0F);
    vec3 one_fifty = vec3(1.0F, 0.0F, 0.5F);

    vec3 csb = ninety + one_fifty;
    vec3 sbo = ninety - one_fifty;
    vec3 inv_sbo = -sbo;

    vec3 left  = csb + inv_sbo;
    vec3 right = csb - inv_sbo;

    float min_antenna_pos = (float(antenna_pairs) - 0.5F) * antenna_spacing_m;

    for (int i = 0; i < antenna_pairs * 2; ++i) {
        vec2  antenna_i_pos = vec2(0.0F, -min_antenna_pos + antenna_spacing_m * float(i));
        float i_dist        = length(position - antenna_i_pos);

        float i_phase = pi / 2.0F;
        if (antenna_i_pos.y > 0.0F) {
            i_phase = -pi / 2.0F;
        }

        vec2 i_value_carrier_c = value(position, antenna_i_pos, loc_freq, 0.0F);
        vec2 i_value_carrier_s = value(position, antenna_i_pos, loc_freq, i_phase);

        for (int j = i; j < antenna_pairs * 2; ++j) {
            if (i != j) {
                vec2  antenna_j_pos = vec2(0.0F, -min_antenna_pos + antenna_spacing_m * float(j));
                float j_dist        = length(position - antenna_j_pos);

                float j_phase = pi / 2.0F;
                if (antenna_j_pos.y > 0.0F) {
                    j_phase = -pi / 2.0F;
                }

                vec2 j_value_carrier_c = value(position, antenna_j_pos, loc_freq, 0.0F);
                vec2 j_value_carrier_s = value(position, antenna_j_pos, loc_freq, j_phase);

                float csb_strength = (i_value_carrier_c + j_value_carrier_c).x;
                float sbo_strength = (i_value_carrier_s + j_value_carrier_s).x;

                signal += csb * csb_strength;
                signal += sbo * sbo_strength;
            }
        }
    }

    vec3 output_color = (signal * output_scale) + output_scale;

    frag_color = vec4(output_color, 1.0F);
}
