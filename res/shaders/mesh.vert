#version 450

#include "utils/vert_uniforms.glsl"

// Inputs
in vec3 local_position;
in vec3 local_normal;
in vec2 local_uv_coords;
in vec3 local_color;

// Outputs
out vec3 world_position;
out vec3 world_normal;
out vec2 uv_coords;
out vec3 vertex_color;

// Logic
void main() {
    vec4 world_position4  = model.world_from_local * vec4(local_position, 1.0F);

    world_position = vec3(world_position4);
    world_normal   = model.world_from_local_normals * local_normal;
    uv_coords      = local_uv_coords;
    vertex_color   = local_color;

    gl_Position = camera.clip_from_world * world_position4;
}
