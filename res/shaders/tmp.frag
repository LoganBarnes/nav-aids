#version 410

in vec2 texture_coordinates;

out vec4 out_color;

void main()
{
    out_color = vec4(texture_coordinates, 1.0F, 1.0F);
}
