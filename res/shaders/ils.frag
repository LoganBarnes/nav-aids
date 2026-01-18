#version 410

#define POWER

#if defined(POWER)
const float power = 100000.0F;
#endif

const float c        = 299.792458F;// m/us
const float loc_freq = 110.1F;// MHz

const float pi = 3.14159265359F;

uniform float pixel_size_m = 1.0F;// m
uniform int   antenna_pairs = 1;
uniform float antenna_spacing_m = 5.0F;//m

uniform vec3 output_scale = vec3(0.1F, 0.0F, 0.0F);

//uniform float time_s;
uniform vec2 field_size_pixels;

out vec4 frag_color;

vec2 value(in vec2 position, in vec2 antenna, in float frequency) {
    float dist_meters = length(position - antenna);
    float microseconds = dist_meters / c;
    float phase_angle = frequency * microseconds;
    float radians    = phase_angle * 2.0F * pi;

    #if defined(POWER)
    float intensity = power / (dist_meters * dist_meters);
    #else
    float intensity = 1.0F;
    #endif

    return vec2(cos(radians), sin(radians)) * intensity;
}

void main()
{
    float half_frame_height = field_size_pixels.y * 0.5F;
    vec2  pixel_pos = vec2(gl_FragCoord.x, half_frame_height - gl_FragCoord.y);
    vec2  position  = pixel_pos * pixel_size_m;

    vec3 signal = vec3(1.0F, 0.0F, 0.0F);

    vec2 carrier_c = vec2(0.0F);
    vec2 carrier_s = vec2(0.0F);

    float min_antenna_pos = (float(antenna_pairs) - 0.5F) * antenna_spacing_m;

    for (int i = 0; i < antenna_pairs * 2; ++i) {
        vec2  antenna_i_pos = vec2(0.0F, -min_antenna_pos + antenna_spacing_m * float(i));

        vec2 i_value_carrier_c = value(position, antenna_i_pos, loc_freq);

        for (int j = i; j < antenna_pairs * 2; ++j) {
            if (i != j) {
                vec2  antenna_j_pos = vec2(0.0F, -min_antenna_pos + antenna_spacing_m * float(j));

                vec2 j_value_carrier_c = value(position, antenna_j_pos, loc_freq);

                signal.y += length(i_value_carrier_c + j_value_carrier_c);
                signal.z += length(i_value_carrier_c - j_value_carrier_c);
            }
        }
    }

    vec3 output_color = (signal * output_scale) + output_scale;

    frag_color = vec4(output_color, 1.0F);
}
