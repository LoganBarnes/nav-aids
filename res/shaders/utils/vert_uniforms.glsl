// Uniforms
uniform Camera {
    mat4 clip_from_world = mat4(1.0F);
} camera;

uniform Model {
    mat4 world_from_local         = mat4(1.0F);
    mat3 world_from_local_normals = mat3(1.0F);
//    uint id;
} model;
