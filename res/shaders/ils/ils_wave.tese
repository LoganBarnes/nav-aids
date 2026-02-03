#version 450

const float PI = 3.14159265359F;

const uint ILS_WAVE_FORM_COMBINED = 0;
const uint ILS_WAVE_FORM_SBO      = 1;
const uint ILS_WAVE_FORM_CSB      = 2;
const uint ILS_WAVE_FORM_CARRIER  = 3;

layout(quads, equal_spacing, cw) in;

// varying input from the tessellation control shader
layout(location = 0) in LineInformation {
    vec2  start_position;
    vec2  end_position;
    float carrier_freq;
    vec2  line_segment_t;
} line_in[];

layout(binding = 0) uniform CameraUniforms
{
    mat4 clip_from_world;
} camera;


layout(push_constant) uniform DisplayUniforms
{
    layout(offset = 32) vec4 color;
    float line_width;
    uint  wave_form;
} display;

// varying output to the fragment shader
layout(location = 0) out Display {
    vec4 color;
} display_out;

float wave_function(in float x) {
    vec2  start_pos    = line_in[0].start_position;
    float carrier_freq = line_in[0].carrier_freq;

    float amp   = 0.5F;
    float inner = x * PI;
    float shift = sign(start_pos.y) * PI * 0.5F;

    float carrier         = sin(inner * carrier_freq) * amp;
    float carrier_shifted = sin((inner * carrier_freq) + shift) * amp;

    float ninety    = sin(inner * 0.9F);
    float one_fifty = sin(inner * 1.5F);

    float audio_csb = ninety + one_fifty;
    float audio_sbo = ninety - one_fifty;

    float radio_csb_mod = carrier * audio_csb * 0.2F;
    float radio_csb     = carrier + radio_csb_mod;
    float radio_sbo     = carrier_shifted * audio_sbo * 0.2F;

    if (display.wave_form == ILS_WAVE_FORM_CARRIER)
    {
        return carrier;
    }
    else if (display.wave_form == ILS_WAVE_FORM_CSB)
    {
        return radio_csb;
    }
    else if (display.wave_form == ILS_WAVE_FORM_SBO)
    {
        return radio_sbo;
    }
    else
    {
        return radio_csb + radio_sbo;
    }
}

void main()
{
    vec2  start_pos    = line_in[0].start_position;
    vec2  end_pos      = line_in[0].end_position;
    float carrier_freq = line_in[0].carrier_freq;
    vec2  segment_t    = line_in[0].line_segment_t;

    float x = mix(segment_t.x, segment_t.y, gl_TessCoord.x);
    float y = wave_function(x);

    vec2 tangent      = vec2(0.01F, wave_function(x + 0.005F) - wave_function(x - 0.005F));
    vec2 normal       = normalize(vec2(-tangent.y, tangent.x));
    vec2 width_offset = mix(-normal, normal, gl_TessCoord.y) * display.line_width * 0.5F;

    vec2 line_dir = normalize(end_pos - start_pos);
    vec2 line_perp = vec2(-line_dir.y, line_dir.x);

    vec2 pos = start_pos + (line_dir * x) + (line_perp * y) + width_offset;

    gl_Position = camera.clip_from_world * vec4(pos, 0.0F, 1.0F);

    display_out.color = display.color;
}
