#version 450

const float PI = 3.14159265359F;

// specify number of control points per patch output
// this value controls the size of the input and output arrays
layout (vertices=2) out;

// varying input from the vertex shader
layout(location = 0) in LineInformation {
    vec2  start_position;
    vec2  end_position;
    float carrier_freq;
    vec2  line_segment_t;
} line_in[];

// varying output to the tessellation evaluation shader
layout(location = 0) out LineInformation {
    vec2  start_position;
    vec2  end_position;
    float carrier_freq;
    vec2  line_segment_t;
} line_out[];

void main()
{
    vec2  start_pos      = line_in[gl_InvocationID].start_position;
    vec2  end_pos        = line_in[gl_InvocationID].end_position;
    float carrier_freq   = line_in[gl_InvocationID].carrier_freq;
    vec2  line_segment_t = line_in[gl_InvocationID].line_segment_t;

    // Pass attributes through
    line_out[gl_InvocationID].start_position = start_pos;
    line_out[gl_InvocationID].end_position   = end_pos;
    line_out[gl_InvocationID].carrier_freq   = carrier_freq;
    line_out[gl_InvocationID].line_segment_t = line_segment_t;

    // Set tess levels:

    float line_length   = abs(line_segment_t.y - line_segment_t.x);
    float tess_level    = ceil(line_length * carrier_freq * 10.0F);

    // The number of isolines
    gl_TessLevelOuter[0] = 1;
    // The number of segments per isoline
    gl_TessLevelOuter[1] = int(tess_level);

    // gl_TessLevelOuter[2-3] are not used for isolines
    // gl_TessLevelInner is not used for isolines

}
