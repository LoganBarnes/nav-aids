#pragma once

// project
#include "ltb/utils/types.hpp"

// external
#include <glm/glm.hpp>

namespace ltb::math::cam
{

struct CameraSettings
{
    enum class AspectType
    {
        KeepHeight,
        KeepWidth,
    };

    enum class ProjectionType
    {
        Perspective,
        Orthographic,
    };

    struct Perspective
    {
        float32 fov_degrees = 60.0F;
    };

    struct Orthographic
    {
        float32 half_size        = 10.0F;
        bool    link_size_to_fov = true;
    };

    AspectType     aspect_type     = AspectType::KeepHeight;
    ProjectionType projection_type = ProjectionType::Perspective;

    float32 near_plane_distance = 1.0F;
    float32 far_plane_distance  = 1000.0F;

    /// \brief Update the near plane distance each frame based on
    //         the camera position and the bounds of the scene.
    bool dynamic_near_plane = true;

    /// \brief Update the far plane distance each frame based on
    //         the camera position and the bounds of the scene.
    bool dynamic_far_plane = true;

    Perspective  perspective  = { };
    Orthographic orthographic = { };

    glm::vec3 initial_eye_position = { 5.0F, 0.0F, 0.0F };
    glm::vec3 initial_focus_point  = { 0.0F, 0.0F, 0.0F };
    glm::vec3 initial_up_direction = { 0.0F, 0.0F, 1.0F };

    // 0.0 is no lag, 1.0 is full lag.
    float32 movement_lag       = +0.5F;
    float32 zoom_sensitivity   = -0.1F;
    float32 pan_sensitivity    = +0.01F;
    float32 rotate_sensitivity = +1.0F;

    bool z_up = false;
};

} // namespace ltb::math::cam
