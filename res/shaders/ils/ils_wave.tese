#version 450

const float PI = 3.14159265359F;

const uint ILS_WAVE_FORM_COMBINED = 0;
const uint ILS_WAVE_FORM_SBO      = 1;
const uint ILS_WAVE_FORM_CSB      = 2;
const uint ILS_WAVE_FORM_CARRIER  = 3;

const uint ANTENNA_BOTH = 0;
const uint ANTENNA_POS  = 1;
const uint ANTENNA_NEG  = 2;

layout(quads, equal_spacing, cw) in;

// varying input from the tessellation control shader
layout(location = 0) in LineInformation {
    vec2  pos_start;
    vec2  neg_start;
    vec2  end;
    float carrier_freq;
    vec2  line_segment_t;
    uint  antenna;
} line_in[];

layout(binding = 0) uniform CameraUniforms
{
    mat4 clip_from_world;
} camera;


layout(push_constant) uniform DisplayUniforms
{
    layout(offset = 48) vec4 color;
    float line_width;
    uint  wave_form;
} display;

// varying output to the fragment shader
layout(location = 0) out Display {
    vec4 color;
} display_out;

float wave_function(in float x, in float shift_sign) {
    float carrier_freq = line_in[0].carrier_freq;

    float amp   = 0.5F;
    float inner = x * PI;
    float shift = shift_sign * PI * 0.5F;

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

float antenna_function(in float x)
{
    if (line_in[0].antenna == ANTENNA_POS)
    {
        return wave_function(x, +1.0F);
    }
    else if (line_in[0].antenna == ANTENNA_NEG)
    {
        return wave_function(x, -1.0F);
    }
    else
    {
        // ANTENNA_BOTH
        return wave_function(x, +1.0F) + wave_function(x, -1.0F);
    }
}

vec2 line_start()
{
    if (line_in[0].antenna == ANTENNA_POS)
    {
        return line_in[0].pos_start;
    }
    else if (line_in[0].antenna == ANTENNA_NEG)
    {
        return line_in[0].neg_start;
    }
    else
    {
        // ANTENNA_BOTH
        return line_in[0].end;
    }
}

vec2 line_end()
{
    if (line_in[0].antenna == ANTENNA_BOTH)
    {
        return line_in[0].end + (normalize(line_in[0].end) * 20.0F);
    }
    else
    {
        return line_in[0].end;
    }
}

void main()
{
    vec2  segment_t = line_in[0].line_segment_t;

    float x = mix(segment_t.x, segment_t.y, gl_TessCoord.x);
    float y = antenna_function(x);

    vec2 tangent      = vec2(0.01F, antenna_function(x + 0.005F) - antenna_function(x - 0.005F));
    vec2 normal       = normalize(vec2(-tangent.y, tangent.x));
    vec2 width_offset = mix(-normal, normal, gl_TessCoord.y) * display.line_width * 0.5F;

    vec2 start     = line_start();
    vec2 end       = line_end();
    vec2 line_dir  = normalize(end - start);
    vec2 line_perp = vec2(-line_dir.y, line_dir.x);

    vec2 pos = start + (line_dir * x) + (line_perp * y) + width_offset;

    gl_Position = camera.clip_from_world * vec4(pos, 0.0F, 1.0F);

    display_out.color = display.color;
}
