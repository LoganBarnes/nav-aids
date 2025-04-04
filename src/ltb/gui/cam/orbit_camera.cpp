#include "ltb/gui/cam/orbit_camera.hpp"

// project
#include "ltb/math/transforms.hpp"

// external
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace ltb::gui::cam
{

auto OrbitCamera::render_params( ) const -> math::cam::CameraRenderParams const&
{
    return render_params_;
}

auto OrbitCamera::settings( ) const -> math::cam::CameraSettings const&
{
    return settings_;
}

auto OrbitCamera::set_settings( math::cam::CameraSettings settings ) -> void
{
    settings_ = settings;
    update( );
}

auto OrbitCamera::set_viewport_size( glm::vec2 viewport_size ) -> void
{
    viewport_size_ = viewport_size;
    update( );
}

auto OrbitCamera::set_world_offset( glm::vec3 const world_offset ) -> void
{
    world_offset_ = world_offset;
}

auto OrbitCamera::set_world_rotation( float const w, float const x, float const y, float const z )
    -> void
{
    world_rotation_ = glm::fquat( w, x, y, z );
}

auto OrbitCamera::handle_inputs( CameraInputSettings const& settings ) -> void
{
    using namespace utils::flag_operators;

    auto const& io = ImGui::GetIO( );

    if ( io.WantCaptureMouse || io.WantCaptureKeyboard )
    {
        return;
    }

    auto button_clicked = [ &io ]( auto button, auto modifiers ) {
        return button != ImGuiMouseButton_COUNT // Check for valid button type
            && io.MouseClicked[ button ] // Check if button is clicked
            && ( !modifiers // Check if modifiers are specified
                 || ( io.KeyMods & modifiers ) ); // Check if modifiers are pressed
    };

    // Pan
    if ( button_clicked( settings.pan_button, settings.pan_modifiers ) )
    {
        movement_modes_ = utils::add_flag( movement_modes_, MovementMode::Pan );
    }
    else if ( io.MouseReleased[ settings.pan_button ] )
    {
        movement_modes_ = utils::remove_flag( movement_modes_, MovementMode::Pan );
    }

    // Rotate (pitch/yaw)
    if ( button_clicked( settings.rotate_button, settings.rotate_modifiers ) )
    {
        movement_modes_ = utils::add_flag( movement_modes_, MovementMode::Rotate );
    }
    else if ( io.MouseReleased[ settings.rotate_button ] )
    {
        movement_modes_ = utils::remove_flag( movement_modes_, MovementMode::Rotate );
    }

    // Handle mouse-drag pan/rotate
    if ( io.MouseDelta.x != 0.f || io.MouseDelta.y != 0.f )
    {
        // Pan
        if ( utils::has_flag( movement_modes_, MovementMode::Pan ) )
        {
            orbit_point_ -= io.MouseDelta.x * render_params_.right * settings_.pan_sensitivity;
            orbit_point_ += io.MouseDelta.y * render_params_.up * settings_.pan_sensitivity;
        }

        // Rotate (pitch/yaw)
        if ( utils::has_flag( movement_modes_, MovementMode::Rotate ) )
        {
            yaw_degrees_ -= io.MouseDelta.x * settings_.rotate_sensitivity;
            pitch_degrees_ -= io.MouseDelta.y * settings_.rotate_sensitivity;
            pitch_degrees_ = glm::clamp( pitch_degrees_, -89.f, +89.f );
        }
    }

    // Zoom
    if ( io.MouseWheel != 0.f )
    {
        orbit_distance_ += io.MouseWheel * settings_.zoom_sensitivity * orbit_distance_;
        orbit_distance_ = glm::max( 0.01f, orbit_distance_ ); // Prevent negative or zero distance
    }

    // Optional: roll left/right when pressing Q/E
    if ( io.KeysDown[ ImGuiKey_Q ] )
    {
        roll_degrees_ -= settings_.rotate_sensitivity;
    }
    if ( io.KeysDown[ ImGuiKey_E ] )
    {
        roll_degrees_ += settings_.rotate_sensitivity;
    }

    // Reset position (and optionally orientation)
    if ( io.KeysDown[ ImGuiKey_R ] )
    {
        orbit_point_   = { };
        pitch_degrees_ = 0.f;
        yaw_degrees_   = 0.f;
        roll_degrees_  = 0.f; // Reset roll as well
    }

    update( );
}

auto OrbitCamera::update( ) -> void
{
    render_params_ = { };

    // Y-up by default; Z-up if specified.
    auto eye   = glm::vec3( 0.f, 0.f, +1.f );
    auto look  = glm::vec3( 0.f, 0.f, -1.f );
    auto up    = glm::vec3( 0.f, +1.f, 0.f );
    auto right = glm::vec3( +1.f, 0.f, 0.f );

    if ( settings_.z_up )
    {
        eye   = glm::vec3( 0.f, -1.f, 0.f );
        look  = glm::vec3( 0.f, +1.f, 0.f );
        up    = glm::vec3( 0.f, 0.f, +1.f );
        right = glm::vec3( +1.f, 0.f, 0.f );
    }

    // 1) Rotate by yaw around the up axis.
    auto yaw_mat = glm::rotate( glm::mat4( 1.f ), glm::radians( yaw_degrees_ ), up );
    // 2) Rotate by pitch around the camera's right axis (after yaw).
    auto pitched_right = math::transform_vector( yaw_mat, right );
    auto pitch_mat = glm::rotate( glm::mat4( 1.f ), glm::radians( pitch_degrees_ ), pitched_right );

    // Combine yaw + pitch
    auto yaw_pitch_mat = pitch_mat * yaw_mat;

    // 3) Roll around the new forward (look) axis after yaw/pitch
    auto new_look = math::transform_vector( yaw_pitch_mat, look );
    new_look      = glm::normalize( new_look );
    auto roll_mat = glm::rotate( glm::mat4( 1.f ), glm::radians( roll_degrees_ ), new_look );
    auto rotation = glm::toMat4( world_rotation_ ) * roll_mat * yaw_pitch_mat;

    // Calculate final vectors
    auto final_eye
        = math::transform_point( rotation, eye * orbit_distance_ ) + orbit_point_ + world_offset_;
    auto final_look  = math::transform_vector( rotation, look );
    auto final_up    = math::transform_vector( rotation, up );
    auto final_right = math::transform_vector( rotation, right );

    render_params_.eye   = final_eye;
    render_params_.look  = final_look;
    render_params_.up    = final_up;
    render_params_.right = final_right;

    // Build perspective
    auto aspect_ratio = ( viewport_size_.y <= 0.f ) ? 1.f : ( viewport_size_.x / viewport_size_.y );
    render_params_.clip_from_view = glm::perspective(
        glm::radians( settings_.perspective.fov_degrees ),
        aspect_ratio,
        settings_.near_plane_distance,
        settings_.far_plane_distance
    );

    // Build view matrix
    render_params_.view_from_world
        = glm::lookAt( final_eye, orbit_point_ + world_offset_, final_up );

    // Combine
    render_params_.clip_from_world = render_params_.clip_from_view * render_params_.view_from_world;
}

} // namespace ltb::gui::cam
