#pragma once

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
        float fov_degrees = 60.f;
    };

    struct Orthographic
    {
        float half_size        = 10.f;
        bool  link_size_to_fov = true;
    };

    AspectType     aspect_type     = AspectType::KeepHeight;
    ProjectionType projection_type = ProjectionType::Perspective;

    float near_plane_distance = 1.f;
    float far_plane_distance  = 1000.f;

    /// \brief Update the near plane distance each frame based on
    //         the camera position and the bounds of the scene.
    bool dynamic_near_plane = true;

    /// \brief Update the far plane distance each frame based on
    //         the camera position and the bounds of the scene.
    bool dynamic_far_plane = true;

    Perspective  perspective  = { };
    Orthographic orthographic = { };

    glm::vec3 initial_eye_position = { 5.f, 0.f, 0.f };
    glm::vec3 initial_focus_point  = { 0.f, 0.f, 0.f };
    glm::vec3 initial_up_direction = { 0.f, 0.f, 1.f };

    float movement_lag       = +0.5f; // 0.0 is no lag, 1.0 is full lag.
    float zoom_sensitivity   = -0.1f;
    float pan_sensitivity    = +0.01f;
    float rotate_sensitivity = +1.f;

    bool z_up = false;
};

} // namespace ltb::math::cam
