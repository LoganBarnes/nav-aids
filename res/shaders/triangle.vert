#version 450

// Constants

// Uniforms
layout(binding = 0) uniform UniformBufferObject {
    mat4 world_from_local;
    mat4 clip_from_world;
} ubo;

// Inputs
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_tex_coords;
layout(location = 3) in vec3 in_color;

// Outpus
layout(location = 0) out vec3 out_color;
layout(location = 1) out vec2 out_tex_coords;

// Logic
void main() {
    gl_Position     = ubo.clip_from_world * ubo.world_from_local * vec4(in_position, 1.f);
    out_color       = in_color;
    out_tex_coords  = vec2(in_tex_coords.x, in_tex_coords.y);
}
