#version 410

const float pi             = 3.14159265359F;
const float speed_of_sound = 343.0F;// m/s

const float speed_of_light = 299792458.0F;// m/s
const float speed_of_light_approx = 300.0F;// m/s
const float mhz  = 1000000F;// Hz
//const float freq = 109100000F;// Hz
const float freq = 109.1F;// Hz

uniform float pixel_size_m = 1.0F;// m

uniform int   antenna_pairs = 1;
uniform float antenna_spacing_m = 5.0F;//m

uniform vec3 output_scale = vec3(0.1F, 0.0F, 0.0F);
//uniform vec3 output_scale = vec3(0.0F, 0.1F, 0.0F);
//uniform vec3 output_scale = vec3(0.0F, 0.0F, 0.1F);

uniform float time_s;
uniform vec2  field_size_pixels;

out vec4 frag_color;

float radio_value(in vec2 position, in vec2 antenna, in float frequency) {
    float dist = length(position - antenna);
    return sin(2.0F * pi * (dist * frequency / speed_of_light_approx - time_s));
}

float audio_value(in vec2 position, in vec2 antenna, in float frequency) {
    float dist = length(position - antenna);
    return sin(2.0F * pi * (dist * frequency / speed_of_sound - time_s));
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

        float i_val     = radio_value(position, antenna_i_pos, freq);
        // float i_val_150 = audio_value(position, antenna_i_pos, 150.0F);
        // float i_val_90  = audio_value(position, antenna_i_pos, 90.0F);
        // float icsb      = i_val + i_val_90 + i_val_150;
        // float isbo      = i_val + i_val_90 - i_val_150;

        for (int j = i; j < antenna_pairs * 2; ++j) {
            if (i != j) {
                vec2 antenna_j_pos = vec2(0.0F, -min_antenna_pos + antenna_spacing_m * float(j));

                float j_val     = radio_value(position, antenna_j_pos, freq);
                // float j_val_150 = audio_value(position, antenna_j_pos, 150.0F);
                // float j_val_90  = audio_value(position, antenna_j_pos, 90.0F);
                // float jcsb      = j_val + j_val_90 + j_val_150;
                // float jsbo      = j_val - j_val_90 + j_val_150;

                // csb += icsb + jcsb;
                // sbo += isbo + jsbo;

                csb += i_val + j_val;
                sbo += i_val - j_val;
            }
        }
    }

    vec3 output_color = (vec3(csb, sbo, csb + sbo) * output_scale) + output_scale;

    frag_color = vec4(output_color, 1.0F);
}
