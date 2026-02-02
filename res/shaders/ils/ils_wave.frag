#version 450

// varying input from the tessellation evaluation shader
layout(location = 0) in Display {
    vec3 color;
} display;

layout(location = 0) out vec4 out_color;

void main()
{
    out_color = vec4(display.color, 1.0F);
}
