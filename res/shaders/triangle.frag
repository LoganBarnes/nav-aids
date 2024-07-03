#version 450

// Constants

// Uniforms
layout(binding = 1) uniform sampler2D tex_sampler;

// Inputs
layout(location = 0) in vec3 in_color;
layout(location = 1) in vec2 in_tex_coords;

// Outputs
layout(location = 0) out vec4 out_color;

// Logic
void main() {
    vec3 tex_color = texture(tex_sampler, in_tex_coords).rgb;
    out_color      = vec4(tex_color * in_color, 1.0f);
}
