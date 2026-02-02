#version 450

const float PI = 3.14159265359F;

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
} display;

// varying output to the fragment shader
layout(location = 0) out Display {
    vec4 color;
} display_out;

void main()
{
    vec2  start_pos    = line_in[0].start_position;
    vec2  end_pos      = line_in[0].end_position;
    float carrier_freq = line_in[0].carrier_freq;
    vec2  segment_t    = line_in[0].line_segment_t;

    float x = mix(segment_t.x, segment_t.y, gl_TessCoord.x);
    float y = sin(x * carrier_freq) * 0.5F;

    vec2 normal = vec2(-cos(x * carrier_freq) * 0.5F, 1.0F / carrier_freq);
    normal = normalize(normal);
    vec2 width_offset = mix(-normal, normal, gl_TessCoord.y) * display.line_width * 0.5F;

    vec2 line_dir = normalize(end_pos - start_pos);
    vec2 line_perp = vec2(-line_dir.y, line_dir.x);

    vec2 pos = start_pos + (line_dir * x) + (line_perp * y) + width_offset;

    gl_Position = camera.clip_from_world * vec4(pos, 0.0F, 1.0F);

    display_out.color = display.color;
}
