#include "ltb/gui/cam/arcball_camera.hpp"

// external
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

namespace ltb::gui::cam
{
namespace
{

auto aspect_ratio( glm::vec2 const& value ) -> float
{
    return value.x / value.y;
}

auto half_size_from_fov( float fov_rad ) -> float
{
    return std::tan( fov_rad * 0.5f );
}

[[nodiscard]] auto
camera_plane_half_size( math::cam::CameraSettings const& settings, float aspect_ratio ) -> glm::vec2
{
    using enum math::cam::CameraSettings::AspectType;
    if ( settings.aspect_type == KeepWidth )
    {
        auto half_size = half_size_from_fov( glm::radians( settings.perspective.fov_degrees ) );
        return glm::vec2{ half_size, half_size / aspect_ratio } * settings.near_plane_distance;
    }
    else
    {
        assert( settings.aspect_type == KeepHeight );
        return glm::
            vec2{ settings.orthographic.half_size * aspect_ratio, settings.orthographic.half_size };
    }
}

auto ndc_to_arcball( glm::vec2 const& p ) -> glm::quat
{
    float dist = glm::dot( p, p );

    // Point is on sphere
    if ( dist <= 1.0F )
    {
        return { 0.0F, { p.x, p.y, std::sqrt( 1.0F - dist ) } };
    }

    // Point is outside sphere
    glm::vec2 proj = glm::normalize( p );
    return { 0.0F, { proj.x, proj.y, 0.0F } };
}

} // namespace

ArcballCamera::ArcballCamera( )
{
    update( );
}

auto ArcballCamera::handle_inputs( CameraInputSettings const& settings ) -> bool
{
    bool need_update = false;

    auto& io = ImGui::GetIO( );

    if ( io.WantCaptureMouse || io.WantCaptureKeyboard )
    {
        return need_update;
    }

    auto mouse_pos_viewport = glm::vec2{ io.MousePos.x, io.MousePos.y };
    auto mouse_pos_norm     = mouse_pos_viewport / viewport_size_;
    curr_mouse_pos_ndc_     = { mouse_pos_norm.x * 2.0F - 1.0F, 1.0F - mouse_pos_norm.y * 2.0F };

    auto button_clicked = [ &io ]( auto button, auto modifiers ) {
        return button != ImGuiMouseButton_COUNT // Check for valid button type
            && io.MouseClicked[ button ] // Check if button is clicked
            && ( !modifiers // Check if modifiers are specified
                 || ( io.KeyMods & modifiers ) ); // Check if modifiers are pressed
    };

    if ( button_clicked( settings.pan_button, settings.pan_modifiers ) )
    {
        movement_modes_ = utils::add_flag( movement_modes_, MovementMode::Pan );
    }
    else if ( ImGui::IsMouseReleased( settings.pan_button ) )
    {
        movement_modes_ = utils::remove_flag( movement_modes_, MovementMode::Pan );
    }

    if ( button_clicked( settings.rotate_button, settings.rotate_modifiers ) )
    {
        movement_modes_ = utils::add_flag( movement_modes_, MovementMode::Rotate );
    }
    else if ( ImGui::IsMouseReleased( settings.rotate_button ) )
    {
        movement_modes_ = utils::remove_flag( movement_modes_, MovementMode::Rotate );
    }

    if ( io.MouseDelta.x != 0.0F || io.MouseDelta.y != 0.0F )
    {
        if ( utils::has_flag( movement_modes_, MovementMode::Pan ) )
        {
            translate( );
            prev_mouse_pos_ndc_ = curr_mouse_pos_ndc_;
            need_update         = true;
        }
        if ( utils::has_flag( movement_modes_, MovementMode::Rotate ) )
        {
            rotate( );
            need_update = true;
        }
    }

    if ( io.MouseWheel != 0.0F )
    {
        zoom(
            io.MouseWheel * camera_settings_.zoom_sensitivity * std::max( target_zooming_, 1E-3F )
        );
        need_update = true;
    }

    // Reset position to the origin.
    if ( io.KeysDown[ GLFW_KEY_R ] )
    {
        target_position_ = { };
        need_update      = true;
    }

    prev_mouse_pos_ndc_ = curr_mouse_pos_ndc_;

    return need_update;
}

auto ArcballCamera::update_camera_settings( ) -> void
{
    if ( camera_settings_.orthographic.link_size_to_fov )
    {
        camera_settings_.orthographic.half_size
            = current_zooming_
            * half_size_from_fov( glm::radians( camera_settings_.perspective.fov_degrees ) );
    }

    if ( viewport_size_.x != 0.0F && viewport_size_.y != 0.0F )
    {
        auto const near_plane_size
            = ArcballCamera::near_plane_size( camera_settings_, viewport_size_ );
        update_projection_matrix( near_plane_size );
    }
}

auto ArcballCamera::reset( ) -> void
{
    update_camera_settings( );

    target_position_        = camera_settings_.initial_focus_point;
    auto const eye_position = camera_settings_.initial_eye_position;
    auto const up_direction = camera_settings_.initial_up_direction;

    auto const unnormalized_look_direction = target_position_ - eye_position;
    auto const z_axis                      = -glm::normalize( unnormalized_look_direction );
    auto const x_axis = glm::normalize( glm::cross( glm::normalize( up_direction ), z_axis ) );
    auto const y_axis = glm::normalize( glm::cross( z_axis, x_axis ) );

    // Debug{} << "x_axis: " << x_axis;
    // Debug{} << "y_axis: " << y_axis;
    // Debug{} << "z_axis: " << z_axis;

    target_zooming_ = glm::length( unnormalized_look_direction );
    target_rotation_
        = glm::normalize( glm::quat_cast( glm::transpose( glm::mat3{ x_axis, y_axis, z_axis } ) ) );
}

auto ArcballCamera::update( ) -> bool
{
    update_camera_settings( );

    auto diff_center   = target_position_ - current_position_;
    auto diff_rotation = target_rotation_ - current_rotation_;
    auto diff_zooming  = target_zooming_ - current_zooming_;

    auto dcenter   = glm::dot( diff_center, diff_center );
    auto drotation = glm::dot( diff_rotation, diff_rotation );
    auto dzooming  = diff_zooming * diff_zooming;

    auto super_small = 1E-10F;
    auto small       = 1E-6F;

    if ( dcenter < super_small && drotation < super_small && dzooming < super_small )
    {
        // Nothing to change
        return false;
    }

    if ( dcenter < small && drotation < small && dzooming < small )
    {
        // Nearly done: just jump directly to the target
        current_position_ = target_position_;
        current_rotation_ = target_rotation_;
        current_zooming_  = target_zooming_;
    }
    else
    {
        current_position_
            = glm::mix( target_position_, current_position_, camera_settings_.movement_lag );
        current_rotation_
            = glm::slerp( target_rotation_, current_rotation_, camera_settings_.movement_lag );
        current_zooming_
            = glm::mix( target_zooming_, current_zooming_, camera_settings_.movement_lag );
    }

    update_view_matrix( );
    return true;
}

auto ArcballCamera::render_params( ) -> math::cam::CameraRenderParams const&
{
    return render_params_;
}

auto ArcballCamera::settings( ) const -> math::cam::CameraSettings const&
{
    return camera_settings_;
}

auto ArcballCamera::set_settings( math::cam::CameraSettings settings ) -> void
{
    camera_settings_ = std::move( settings );
}

auto ArcballCamera::set_viewport_size( glm::vec2 const viewport_size ) -> void
{
    viewport_size_ = viewport_size;
}

auto ArcballCamera::near_plane_size(
    math::cam::CameraSettings const& settings,
    glm::vec2 const&                 viewport
) -> glm::vec2
{
    return camera_plane_half_size( settings, aspect_ratio( viewport ) ) * 2.0F;
}

auto ArcballCamera::rotate( ) -> void
{
    glm::quat prev_rot = ndc_to_arcball( prev_mouse_pos_ndc_ );
    glm::quat curr_rot = ndc_to_arcball( curr_mouse_pos_ndc_ );

    target_rotation_ = glm::normalize( curr_rot * prev_rot * target_rotation_ );
}

auto ArcballCamera::translate( ) -> void
{
    glm::vec2 translation_ndc = curr_mouse_pos_ndc_ - prev_mouse_pos_ndc_;

    float half_size = camera_settings_.orthographic.half_size;

    if ( !camera_settings_.orthographic.link_size_to_fov
         && camera_settings_.projection_type
                == math::cam::CameraSettings::ProjectionType::Perspective )
    {
        half_size = half_size_from_fov( glm::radians( camera_settings_.perspective.fov_degrees ) );
    }

    float aspect = aspect_ratio( viewport_size_ );

    switch ( camera_settings_.aspect_type )
    {
        using enum math::cam::CameraSettings::AspectType;
        case KeepWidth:
            translation_ndc.x *= half_size;
            translation_ndc.y *= half_size / aspect;
            break;

        case KeepHeight:
            translation_ndc.x *= aspect * half_size;
            translation_ndc.y *= half_size;
            break;

    } // end switch

    target_position_ -= inverse_rotation_ * glm::vec3{ translation_ndc.x, translation_ndc.y, 0.0F };
}

auto ArcballCamera::zoom( float amount ) -> void
{
    auto link_to_target_zoom = [ this ] {
        if ( camera_settings_.orthographic.link_size_to_fov )
        {
            camera_settings_.orthographic.half_size
                = target_zooming_
                * half_size_from_fov( glm::radians( camera_settings_.perspective.fov_degrees ) );
        }
    };

    auto to_zoom_plane = [ this, &link_to_target_zoom ]( glm::vec2 ndc_pos ) {
        link_to_target_zoom( );
        glm::vec2 half_plane_size
            = camera_plane_half_size( camera_settings_, aspect_ratio( viewport_size_ ) );
        return half_plane_size * ndc_pos;
    };

    auto from_zoom_plane = [ this, &link_to_target_zoom ]( glm::vec2 plane_pos ) {
        link_to_target_zoom( );
        glm::vec2 half_plane_size
            = camera_plane_half_size( camera_settings_, aspect_ratio( viewport_size_ ) );
        return plane_pos / half_plane_size;
    };

    target_zoom_pos_ = to_zoom_plane( curr_mouse_pos_ndc_ );

    target_zooming_ += amount;
    target_zooming_ = std::max( target_zooming_, 0.0F );

    prev_mouse_pos_ndc_ = from_zoom_plane( target_zoom_pos_ );

    translate( );
}

auto ArcballCamera::update_projection_matrix( glm::vec2 const& near_plane_size ) -> void
{
    auto half_size = near_plane_size * 0.5f;

    switch ( camera_settings_.projection_type )
    {
        using enum math::cam::CameraSettings::ProjectionType;
        case Perspective:
            render_params_.clip_from_view = glm::perspective(
                glm::radians( camera_settings_.perspective.fov_degrees ),
                aspect_ratio( near_plane_size ),
                camera_settings_.near_plane_distance,
                camera_settings_.far_plane_distance
            );
            break;

        case Orthographic:
            render_params_.clip_from_view = glm::ortho(
                -half_size.x,
                +half_size.x,
                -half_size.y,
                +half_size.y,
                camera_settings_.near_plane_distance,
                camera_settings_.far_plane_distance
            );
            break;
    } // end switch

    render_params_.clip_from_world = render_params_.clip_from_view * render_params_.view_from_world;
}

auto ArcballCamera::update_view_matrix( ) -> void
{
    inverse_rotation_ = glm::inverse( current_rotation_ );
    auto up_dir       = inverse_rotation_ * glm::vec3{ 0, 1, 0 };
    auto eye_position = inverse_rotation_ * glm::vec3{ 0, 0, current_zooming_ };
    auto look_dir     = inverse_rotation_ * glm::vec3{ 0, 0, -1 };

    eye_position += current_position_;

    // Debug{} << "eye, center, up";
    // Debug{} << eye_position;
    // Debug{} << current_position_;
    // Debug{} << up_dir;

    render_params_.view_from_world
        = glm::lookAt( eye_position, current_position_ + look_dir, up_dir );
    render_params_.clip_from_world = render_params_.clip_from_view * render_params_.view_from_world;
}

} // namespace ltb::gui::cam
