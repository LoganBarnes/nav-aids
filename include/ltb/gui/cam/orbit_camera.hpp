#pragma once

// project
#include "ltb/gui/cam/camera_input_settings.hpp"
#include "ltb/math/cam/camera_render_params.hpp"
#include "ltb/math/cam/camera_settings.hpp"
#include "ltb/utils/enum_flags.hpp"
#include "ltb/utils/types.hpp"

// external
#include <glm/detail/type_quat.hpp>

namespace ltb::gui::cam
{

enum class MovementMode
{
    Pan,
    Rotate,
    Zoom,
};

class OrbitCamera
{
public:
    OrbitCamera( ) = default;

    auto handle_inputs( CameraInputSettings const& settings ) -> void;

    auto render_params( ) const -> math::cam::CameraRenderParams const&;
    auto settings( ) const -> math::cam::CameraSettings const&;

    auto set_settings( math::cam::CameraSettings settings ) -> void;
    auto set_viewport_size( glm::vec2 viewport_size ) -> void;

    auto set_world_offset( glm::vec3 world_offset ) -> void;

    auto set_world_rotation( float32 w, float32 x, float32 y, float32 z ) -> void;

private:
    math::cam::CameraSettings     settings_      = { };
    math::cam::CameraRenderParams render_params_ = { };

    utils::Flags< MovementMode > movement_modes_ = { };

    glm::fquat world_rotation_ = glm::fquat( );
    glm::vec3  world_offset_   = { 0.0F, 0.0F, 0.0F };
    glm::vec3  orbit_point_    = { 0.0F, 0.0F, 0.0F };
    float32    pitch_degrees_  = 0.0F;
    float32    yaw_degrees_    = 0.0F;

    float32   orbit_distance_ = 50.0F;
    glm::vec2 viewport_size_  = { 1.0F, 1.0F };

    auto update( ) -> void;
};

} // namespace ltb::gui::cam
