#version 450

#include "utils/vert_uniforms.glsl"

layout(push_constant) uniform Model {
    mat4 world_from_local;
    mat3 world_from_local_normals;
//    uint id;
} model;

// Inputs
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_vertex_color;

// Outputs
layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_uv_coords;
layout(location = 3) out vec3 out_vertex_color;
//layout(location = 4) out flat uint out_id;

// Logic
void main() {
    vec4 world_position = model.world_from_local * vec4(in_position, 1.f);

    out_position     = vec3(world_position);
    out_normal       = vec3(0.f);
    out_uv_coords    = vec2(0.f);
    out_vertex_color = in_vertex_color;
    // out_id           = model.id;

    gl_Position = camera.clip_from_world * world_position;
}
