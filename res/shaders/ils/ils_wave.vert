#version 450

layout(push_constant) uniform LineInformation {
    vec2  start_position;
    vec2  end_position;
    float carrier_freq;
    uint  num_segments;
} line_in;

// varying output to tessellation control shader
layout(location = 0) out LineInformation {
    vec2  start_position;
    vec2  end_position;
    float carrier_freq;
    vec2  line_segment_t;
} line_out;

void main()
{
    float line_length    = distance(line_in.start_position, line_in.end_position);
    float segment_length = line_length / float(line_in.num_segments);

    line_out.start_position = line_in.start_position;
    line_out.end_position   = line_in.end_position;
    line_out.carrier_freq   = line_in.carrier_freq;
    line_out.line_segment_t = vec2(gl_VertexIndex, gl_VertexIndex + 1) * segment_length;
}
