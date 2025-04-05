#version 410

// Inputs
//in vec3 world_position;
//in vec3 world_normal;
//in vec2 uv_coords;
//in vec3 vertex_color;

// Outputs
out vec4 out_color;
//
//vec3 get_color() {
//    switch (color_mode) {
//        case color_mode_position:
//        return world_position;
//
//        case color_mode_normal:
//        return normalize(world_normal) * 0.5F + 0.5F;
//
//        case color_mode_uv_coord:
//        return vec3(uv_coords, 1.0F);
//
//        case color_mode_vertex_color:
//        return vertex_color;
//
//        case color_mode_global_color:
//        return uniform_color.rgb;
//
//        case color_mode_texture:
//        return vec3(uv_coords, 1.0F);
//        // return texture(tex_sampler, uv_coords).rgb;
//
//        case color_mode_white:
//        default :
//        break;
//    }
//
//    return vec3(1.0F);
//}
//
//vec3 get_shading() {
//    return vec3(1.0F);
//}

// Logic
void main() {
//    vec3 color   = get_color();
//    vec3 shading = get_shading();

//    out_color = vec4(clamp(color * shading, 0.0F, 1.0F), 1.0F);
    out_color = vec4(1.0F);
}
