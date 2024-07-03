#version 450

#include "utils/frag_constants_and_uniforms.glsl"

// Inputs
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv_coords;
layout(location = 3) in vec3 in_vertex_color;
//layout(location = 4) in flat uint in_id;

// Outputs
layout(location = 0) out vec4 out_color;
//layout(location = 1) out uint out_id;
//layout(location = 2) out vec3 out_position;

vec3 get_color() {
    switch (display.color_mode) {
        case color_mode_position:
        return in_position;

        case color_mode_normal:
        return normalize(in_normal) * 0.5f + 0.5f;

        case color_mode_uv_coord:
        return vec3(in_uv_coords, 1.f);

        case color_mode_vertex_color:
        return in_vertex_color;

        case color_mode_global_color:
        return display.color.rgb;

        case color_mode_texture:
        return vec3(in_uv_coords, 1.f);
        // return texture(tex_sampler, in_uv_coords).rgb;

        case color_mode_white:
        default :
        break;
    }

    return vec3(1.f);
}

vec3 get_shading() {
    return vec3(1.f);
}

// Logic
void main() {
    // out_id       = in_id;
    // out_position = in_position;

    vec3 color   = get_color();
    vec3 shading = get_shading();

    out_color = vec4(clamp(color * shading, 0.f, 1.f), 1.f);
}
