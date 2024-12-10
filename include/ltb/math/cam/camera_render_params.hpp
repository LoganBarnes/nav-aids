#pragma once

// external
#include <glm/glm.hpp>

namespace ltb::math::cam
{

struct CameraRenderParams
{
    glm::vec3 eye   = { +0.0F, +0.0F, +5.0F };
    glm::vec3 look  = { +0.0F, +0.0F, -1.0F };
    glm::vec3 up    = { +0.0F, +1.0F, +0.0F };
    glm::vec3 right = { +1.0F, +0.0F, +0.0F };

    glm::mat4 clip_from_view  = glm::mat4( 1.0F );
    glm::mat4 view_from_world = glm::mat4( 1.0F );
    glm::mat4 clip_from_world = glm::mat4( 1.0F );

    glm::mat3 basis = glm::mat3( 1.0F ); // basis[0], basis[1], basis[2] == u, v, w
};

} // namespace ltb::math::cam
