#version 410

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

float value(in vec2 position, in vec2 antenna, in float frequency) {
    float dist = length(position - antenna);
    return sin(dist * two_pi_over_c_km * frequency);
}

void main()
{
    vec2 pixel_pos = gl_FragCoord.xy - vec2(0.0F, field_size_pixels.y / 2.0F);
    vec2 position  = pixel_pos * pixel_size_m;

    float csb = 0.0F;
    float sbo = 0.0F;

    float min_antenna_pos = (float(antenna_pairs) - 0.5F) * antenna_spacing_m;

    for (int i = 0; i < antenna_pairs * 2; ++i) {
        vec2 antenna_i_pos = vec2(0.0F, -min_antenna_pos + antenna_spacing_m * float(i));

        float i_val     = value(position, antenna_i_pos, loc_freq);

        float i_val_150 = value(position, antenna_i_pos, one_fifty_freq);
        float i_val_90  = value(position, antenna_i_pos, ninety_freq);

        float i_csb      = i_val_90 + i_val_150;
        float i_sbo      = i_val_90 - i_val_150;
        float i_combined = i_csb + i_sbo;
        float i_final    = i_val * (i_combined * 0.5F + 0.5F);

        for (int j = i; j < antenna_pairs * 2; ++j) {
            if (i != j) {
                vec2 antenna_j_pos = vec2(0.0F, -min_antenna_pos + antenna_spacing_m * float(j));

                float j_val     = value(position, antenna_j_pos, loc_freq);

                float j_val_150 = value(position, antenna_j_pos, one_fifty_freq);
                float j_val_90  = value(position, antenna_j_pos, ninety_freq);

                float j_csb      = j_val_90 + j_val_150;
                float j_sbo      = j_val_90 + j_val_150;
                float j_combined = j_csb + j_sbo;
                float j_final    = j_val * (j_combined * 0.5F + 0.5F);

                csb += i_final + j_final;
                sbo += i_final - j_final;
            }
        }
    }

    vec3 output_color = (vec3(csb, sbo, csb + sbo) * output_scale) + output_scale;

    frag_color = vec4(output_color, 1.0F);
}
