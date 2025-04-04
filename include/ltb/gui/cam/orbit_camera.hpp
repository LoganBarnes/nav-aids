#pragma once

// project
#include "ltb/gui/cam/camera_input_settings.hpp"
#include "ltb/math/cam/camera_render_params.hpp"
#include "ltb/math/cam/camera_settings.hpp"
#include "ltb/utils/enum_flags.hpp"

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

    auto set_world_rotation( float w, float x, float y, float z ) -> void;

private:
    math::cam::CameraSettings     settings_      = { };
    math::cam::CameraRenderParams render_params_ = { };

    utils::Flags< MovementMode > movement_modes_ = { };

    glm::fquat world_rotation_ = glm::fquat( );
    glm::vec3  world_offset_   = { 0.f, 0.f, 0.f };
    glm::vec3  orbit_point_    = { 0.f, 0.f, 0.f };
    float      pitch_degrees_  = 0.f;
    float      yaw_degrees_    = 0.f;
    float      roll_degrees_   = 0.f; // NEW: track roll angle

    float     orbit_distance_ = 15.f;
    glm::vec2 viewport_size_  = { 1.f, 1.f };

    auto update( ) -> void;
};

} // namespace ltb::gui::cam
