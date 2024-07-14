#version 410

out vec2 texture_coordinates;

void main()
{
    texture_coordinates    = vec2(0.5F, 0.5F);
    vec2 clip_coordinates  = vec2(0.0F, 0.0F);

    switch (gl_VertexID)
    {
        case 0:
        texture_coordinates = vec2(+0.0F, +1.0F);
        clip_coordinates    = vec2(-1.0F, -1.0F);
        break;

        case 1:
        texture_coordinates = vec2(+1.0F, +1.0F);
        clip_coordinates    = vec2(+1.0F, -1.0F);
        break;

        case 2:
        texture_coordinates = vec2(+0.0F, +0.0F);
        clip_coordinates    = vec2(-1.0F, +1.0F);
        break;

        case 3:
        texture_coordinates = vec2(+1.0F, +0.0F);
        clip_coordinates    = vec2(+1.0F, +1.0F);
        break;

        default :
        break;
    }

    gl_Position = vec4(clip_coordinates, 0.5F, 1.0F);
}
