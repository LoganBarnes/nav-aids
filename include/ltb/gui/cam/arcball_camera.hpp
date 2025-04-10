#pragma once

// project
#include "ltb/gui/cam/camera_input_settings.hpp"
#include "ltb/math/cam/camera_render_params.hpp"
#include "ltb/math/cam/camera_settings.hpp"
#include "ltb/utils/enum_flags.hpp"
#include "ltb/utils/types.hpp"

// external
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// standard
#include <memory>

namespace ltb::gui::cam
{

class ArcballCamera
{
public:
    ArcballCamera( );

    auto handle_inputs( CameraInputSettings const& settings ) -> bool;

    /// \brief
    auto reset( ) -> void;

    auto update( ) -> bool;

    [[nodiscard]] auto render_params( ) -> math::cam::CameraRenderParams const&;
    [[nodiscard]] auto settings( ) const -> math::cam::CameraSettings const&;

    auto set_settings( math::cam::CameraSettings settings ) -> void;
    auto set_viewport_size( glm::vec2 viewport_size ) -> void;

    static auto
    near_plane_size( math::cam::CameraSettings const& settings, glm::vec2 const& viewport )
        -> glm::vec2;

protected:
    enum class MovementMode
    {
        Pan,
        Rotate,
    };

    math::cam::CameraSettings camera_settings_ = { };
    glm::vec2                 viewport_size_   = { };

    math::cam::CameraRenderParams render_params_ = { };

    glm::vec3 target_position_  = { };
    glm::vec3 current_position_ = { };

    glm::quat target_rotation_  = { };
    glm::quat current_rotation_ = { };

    float32   target_zooming_  = 1.0F;
    float32   current_zooming_ = 1.0F;
    glm::vec2 target_zoom_pos_ = { };

    utils::Flags< MovementMode > movement_modes_ = { };

    glm::quat inverse_rotation_   = { };
    glm::vec2 prev_mouse_pos_ndc_ = { };
    glm::vec2 curr_mouse_pos_ndc_ = { };

    auto rotate( ) -> void;
    auto translate( ) -> void;
    auto zoom( float32 amount ) -> void;

    auto update_camera_settings( ) -> void;

    auto update_projection_matrix( glm::vec2 const& near_plane_size ) -> void;
    auto update_view_matrix( ) -> void;
};

} // namespace ltb::gui::cam
