#version 450

const uint ANTENNA_BOTH = 0;
const uint ANTENNA_POS  = 1;
const uint ANTENNA_NEG  = 2;

layout(push_constant) uniform LineInformation {
    vec2  pos_start;// 8 bytes
    vec2  neg_start;// 8 bytes
    vec2  end;// 8 bytes
    float carrier_freq;// 4 bytes
    uint  num_segments;// 4 bytes
    uint  antenna;// 4 bytes
// Total: 24 bytes
} line_in;

// varying output to tessellation control shader
layout(location = 0) out LineInformation {
    vec2  pos_start;
    vec2  neg_start;
    vec2  end;
    float carrier_freq;
    vec2  line_segment_t;
    uint  antenna;
} line_out;

float line_length() {
    if (line_in.antenna == ANTENNA_POS)
    {
        return distance(line_in.pos_start, line_in.end);
    }
    else if (line_in.antenna == ANTENNA_NEG)
    {
        return distance(line_in.neg_start, line_in.end);
    }
    else
    {
        // ANTENNA_BOTH
        return 20.0F;
    }
}

float line_start() {
    if (line_in.antenna == ANTENNA_BOTH)
    {
        return line_in.end.x;
    }
    else
    {
        return 0.0F;
    }
}

void main()
{
    line_out.pos_start    = line_in.pos_start;
    line_out.neg_start    = line_in.neg_start;
    line_out.end          = line_in.end;
    line_out.carrier_freq = line_in.carrier_freq;
    line_out.antenna      = line_in.antenna;

    float segment_length     = line_length() / float(line_in.num_segments);
    line_out.line_segment_t  = vec2(gl_VertexIndex, gl_VertexIndex + 1) * segment_length;
//    line_out.line_segment_t += line_start();
}
