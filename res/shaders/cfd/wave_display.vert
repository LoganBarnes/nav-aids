#version 410 core

in ivec2 index;

uniform sampler2D wave_values;
uniform int       resolution;

out float wave_height;

void main()
{
    float center_pixel_coord = float(index.x) + 0.5F;
    vec2  texture_coords     = vec2(center_pixel_coord / float(resolution), 1.0F);

    wave_height = texture(wave_values, texture_coords).r;

    float edge_pixel_coord = float(index.x + index.y);
    float edge_uv_coord    = edge_pixel_coord / float(resolution);
    vec2  clip_coords      = vec2(edge_uv_coord * 2.0F - 1.0F, wave_height);

    gl_Position = vec4(clip_coords, 0.5F, 1.0F);
}
