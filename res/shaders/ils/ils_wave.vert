#version 450

layout(binding = 0) uniform CameraUniforms
{
    mat4 clip_from_world;
} camera;

layout(push_constant) uniform LineInformation {
    vec2 start_position;
    vec2 end_position;
} line;

//layout(location = 0) out VertexData {
//    vec4 line_start_and_end;
//} vs_out;

void main()
{
    if (gl_VertexIndex == 0) {
        gl_Position = camera.clip_from_world * vec4(line.start_position, 0.0F, 1.0F);
    } else {
        gl_Position = camera.clip_from_world * vec4(line.end_position, 0.0F, 1.0F);
    }
}
