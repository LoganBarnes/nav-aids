// ///////////////////////////////////////////////////////////////////////////////////////
// A Logan Thomas Barnes project
// ///////////////////////////////////////////////////////////////////////////////////////
#include "app.hpp"

// project
#include "ltb/exec/app_defaults.hpp"
#include "ltb/gui/imgui_utils.hpp"
#include "ltb/vlk/buffer_utils.hpp"
#include "ltb/vlk/check.hpp"
#include "ltb/vlk/device_memory_utils.hpp"

// external
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <spdlog/spdlog.h>

namespace ltb
{

IlsApp::IlsApp( window::GlfwContext& glfw_context, window::GlfwWindow& glfw_window )
    : glfw_context_( glfw_context )
    , glfw_window_( glfw_window )
{
}

auto IlsApp::initialize( ) -> utils::Result< exec::UpdateLoopStatus >
{
    if ( !this->is_initialized( ) )
    {

        LTB_CHECK( this->initialize_gpu( )
                       .and_then( &IlsApp::initialize_presentation )
                       .and_then( &IlsApp::initialize_camera )
                       .and_then( &IlsApp::initialize_waves )
                       .and_then( &IlsApp::initialize_display_pipeline )
                       .and_then( &IlsApp::initialize_meshes ) );

        update_world_pos( { 10.0F, 0.0F } );

        initialized_ = true;
    }

    return exec::UpdateLoopStatus{
        .update_time_step = utils::duration_seconds( 0.025F ),
    };
}

auto IlsApp::is_initialized( ) const -> bool
{
    return initialized_;
}

auto IlsApp::fixed_step_update( exec::UpdateLoopStatus const& status ) -> exec::UpdateRequests
{
    utils::ignore( status );
    return { };
}

auto IlsApp::frame_update( exec::UpdateLoopStatus const& status ) -> exec::UpdateRequests
{
    utils::ignore( status );

    imgui_.new_frame( );
    this->configure_gui( );

    // auto display_uniforms = fluid_display_.uniforms( );
    // fluid_display_.set_uniforms( display_uniforms );

    if ( auto result = this->render( ); !result )
    {
        spdlog::error( "render() failed: {}", result.error( ).debug_error_message( ) );
    }

    return { };
}

auto IlsApp::configure_gui( ) -> void
{
    if ( camera_.handle_inputs( ) )
    {
        camera_frames_updated_.clear( );
    }

    utils::ignore(
        ImGui::DockSpaceOverViewport( 0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode )
    );

    if ( ImGui::Begin( "Simulation" ) )
    {
        ImGui::Text( "FPS: %.1F", ImGui::GetIO( ).Framerate );
    }
    ImGui::End( );

    if ( ImGui::IsMouseDown( ImGuiMouseButton_Left ) )
    {
        auto const cam = camera_.simple_render_params( );

        auto const mouse_pos = glm::vec2{ ImGui::GetMousePos( ) };
        auto const clip_pos
            = ( ( mouse_pos / glm::vec2{ ImGui::GetIO( ).DisplaySize } ) * 2.0F ) - 1.0F;
        auto const world_pos
            = glm::vec2( cam.world_from_clip * glm::vec4( clip_pos.x, clip_pos.y, 0.0F, 1.0F ) );

        ImGui::SetTooltip( "World pos: (%.2F, %.2F) m", world_pos.x, world_pos.y );
    }
}

auto IlsApp::on_resize( glm::ivec2 const size ) -> utils::Result< void >
{
    camera_.resize( glm::vec2( size ) );
    camera_frames_updated_.clear( );

    return presentation_.rebuild( {
        .swapchain = presentation_.swapchain( ).settings( ),
    } );
}

auto IlsApp::clean_up( ) -> utils::Result< void >
{
    VK_CHECK( gpu_.device( ).get( ).waitIdle( ) );

    return utils::success( );
}

auto IlsApp::initialize_gpu( ) -> utils::Result< IlsApp* >
{
    auto gpu_settings = vlk::objs::VulkanGpuSettings{ };
    gpu_settings.device.device_features.push_back(
        &vk::PhysicalDeviceFeatures::tessellationShader
    );

    LTB_CHECK( gpu_.initialize( std::move( gpu_settings ) ) );

    LTB_CHECK_VALID( gpu_.device( ).queues( ).contains( vlk::QueueType::Graphics ) );
    LTB_CHECK_VALID( gpu_.device( ).queues( ).contains( vlk::QueueType::Compute ) );
    LTB_CHECK_VALID( gpu_.device( ).queues( ).contains( vlk::QueueType::Surface ) );

    auto const& graphics_queue = gpu_.device( ).queues( ).at( vlk::QueueType::Graphics );
    auto const& compute_queue  = gpu_.device( ).queues( ).at( vlk::QueueType::Compute );
    auto const& surface_queue  = gpu_.device( ).queues( ).at( vlk::QueueType::Surface );

    LTB_CHECK_VALID( graphics_queue == compute_queue );

    graphics_and_compute_queue_ = graphics_queue;
    presentation_queue_         = surface_queue;

    return this;
}

auto IlsApp::initialize_presentation( ) -> utils::Result< IlsApp* >
{
    LTB_CHECK( presentation_.initialize( {
        .render_pass = vlk::render_pass_settings_2d( vk::Format::eUndefined ),
    } ) );

    LTB_CHECK( imgui_.initialize( ) );

    return this;
}

auto IlsApp::initialize_camera( ) -> utils::Result< IlsApp* >
{
    auto camera_buffer_layout = vlk::MemoryLayout{ };
    append_memory_size_n(
        camera_buffer_layout,
        sizeof( cam::SimpleCameraRenderParams ),
        exec::max_frames_in_flight
    );

    LTB_CHECK( camera_ubo_.initialize( {
        .layout       = std::move( camera_buffer_layout ),
        .buffer_usage = vk::BufferUsageFlagBits::eUniformBuffer,
        .memory_properties
        = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        .store_mapped_value = true,
    } ) );

    camera_.set_width( 36.0F );
    camera_.set_center( { 13.0F, 0.0F } );

    return this;
}

auto IlsApp::initialize_waves( ) -> utils::Result< IlsApp* >
{
    LTB_CHECK( ils_wave_pipeline_.initialize( {
        .frame_count = exec::max_frames_in_flight,
        .camera_ubo  = camera_ubo_,
    } ) );

    LTB_CHECK( pos_wave_, ils_wave_pipeline_.initialize_wave( ) );
    LTB_CHECK( neg_wave_, ils_wave_pipeline_.initialize_wave( ) );

    return this;
}

auto IlsApp::initialize_display_pipeline( ) -> utils::Result< IlsApp* >
{
    LTB_CHECK_VALID( camera_ubo_.is_initialized( ) );

    // LTB_CHECK( fluid_display_.initialize( {
    //     .frame_count = exec::max_frames_in_flight,
    //     .camera_ubo  = camera_ubo_,
    // } ) );

    LTB_CHECK( line_display_.initialize( {
        .frame_count = exec::max_frames_in_flight,
        .camera_ubo  = camera_ubo_,
    } ) );

    LTB_CHECK( graphics_cmd_and_sync_.initialize( {
        .frame_count  = exec::max_frames_in_flight,
        .image_count  = static_cast< uint32 >( presentation_.swapchain( ).images( ).size( ) ),
        .command_pool = {
            .queue_type = vlk::QueueType::Graphics,
        },
    } ) );

    return this;
}

auto IlsApp::initialize_meshes( ) -> utils::Result< IlsApp* >
{
    auto grid_lines = vlk::dd::SimpleMesh2{ };

    constexpr auto ils_grid_length_km_count = 30U;
    constexpr auto ils_grid_width_km_count  = 10U;
    constexpr auto ils_grid_length_km       = static_cast< float32 >( ils_grid_length_km_count );
    constexpr auto ils_grid_width_km        = static_cast< float32 >( ils_grid_width_km_count );

    for ( auto i = 0U; i <= ils_grid_length_km_count; ++i )
    {
        auto const     x_pos      = static_cast< float32 >( i );
        constexpr auto half_width = ils_grid_width_km * 0.5F;

        utils::ignore(
            grid_lines.positions.emplace_back( x_pos, -half_width ),
            grid_lines.positions.emplace_back( x_pos, +half_width )
        );
    }
    for ( auto i = 0U; i <= ils_grid_width_km_count; ++i )
    {
        auto const y_pos = static_cast< float32 >( i ) - ( ils_grid_width_km * 0.5F );

        utils::ignore(
            grid_lines.positions.emplace_back( 0.0F, y_pos ),
            grid_lines.positions.emplace_back( ils_grid_length_km, y_pos )
        );
    }

    LTB_CHECK(
        auto* const grid_uniforms,
        line_display_.initialize_mesh(
            grid_lines,
            graphics_cmd_and_sync_.command_pool( ),
            graphics_and_compute_queue_
        )
    );
    grid_uniforms->display.color = { 0.5F, 0.5F, 0.5F, 1.0F };

    LTB_CHECK(
        conversion_line_,
        line_display_.initialize_mesh(
            vlk::dd::SimpleMesh2{ .positions = { { 0.0F, -1.0F }, { 0.0F, +1.0F } } },
            graphics_cmd_and_sync_.command_pool( ),
            graphics_and_compute_queue_
        )
    );
    conversion_line_->display.color = { 1.0F, 1.0F, 1.0F, 1.0F };

    return this;
}

auto IlsApp::compute( ) -> utils::Result< void >
{
    // LTB_CHECK( fluid_compute_.dispatch( ) );

    return utils::success( );
}

auto IlsApp::render( ) -> utils::Result< void >
{
    LTB_CHECK(
        auto const maybe_frame,
        graphics_cmd_and_sync_.start_frame( presentation_.swapchain( ) )
    );

    if ( maybe_frame.has_value( ) )
    {
        auto const& frame = maybe_frame.value( );

        LTB_CHECK( this->update_camera_uniforms( frame ) );
        LTB_CHECK( this->record_render_commands( frame ) );

        auto wait_until_signaled = std::vector{
            vlk::objs::SemaphoreAndStage{
                .semaphore = frame.image_semaphore,
                .stage     = vk::PipelineStageFlagBits::eColorAttachmentOutput,
            },
        };

        LTB_CHECK(
            auto const& render_finished_semaphore,
            graphics_cmd_and_sync_.get_present_semaphore( frame )
        );

        LTB_CHECK( graphics_cmd_and_sync_.end_frame(
            frame,
            wait_until_signaled,
            { render_finished_semaphore },
            graphics_and_compute_queue_
        ) );

        LTB_CHECK( graphics_cmd_and_sync_
                       .present_frame( frame, presentation_.swapchain( ), presentation_queue_ ) );

        graphics_cmd_and_sync_.increment_frame( );
    }

    return utils::success( );
}

auto IlsApp::update_camera_uniforms( vlk::objs::FrameInfo const& frame ) -> utils::Result< void >
{
    if ( camera_frames_updated_.contains( frame.frame_index ) )
    {
        return utils::success( );
    }

    auto const cam_uniforms = camera_.simple_render_params( );

    LTB_CHECK_VALID( frame.frame_index < camera_ubo_.layout( ).ranges.size( ) );
    auto const& memory_range = camera_ubo_.layout( ).ranges[ frame.frame_index ];

    auto* const dst_data = camera_ubo_.mapped_data( ) + memory_range.offset;
    LTB_CHECK_VALID( memory_range.size >= sizeof( cam_uniforms ) );
    LTB_CHECK_VALID( std::memcpy( dst_data, &cam_uniforms, sizeof( cam_uniforms ) ) == dst_data );

    utils::ignore( camera_frames_updated_.emplace( frame.frame_index ) );

    return utils::success( );
}

auto IlsApp::record_render_commands( vlk::objs::FrameInfo const& frame ) -> utils::Result< void >
{
    VK_CHECK( frame.command_buffer.begin( vk::CommandBufferBeginInfo{ } ) );

    LTB_CHECK( presentation_.begin_render_pass( {
        .command_buffer    = frame.command_buffer,
        .image_index       = frame.image_index,
        .color_clear_value = { 0.35F, 0.35F, 0.35F, 1.0F },
    } ) );

    LTB_CHECK( line_display_.draw( frame ) );
    LTB_CHECK( ils_wave_pipeline_.draw( frame ) );

    imgui_.render( frame.command_buffer );

    frame.command_buffer.endRenderPass( );

    VK_CHECK( frame.command_buffer.end( ) );

    return utils::success( );
}

auto IlsApp::update_world_pos( glm::vec2 const world_pos ) -> void
{
    auto const angle = glm::atan( world_pos.y, world_pos.x );

    auto const transform = glm::translate( glm::identity< glm::mat3 >( ), world_pos )
                         * glm::rotate( glm::identity< glm::mat3 >( ), angle );

    conversion_line_->model.transform = glm::mat3x4{ transform };

    auto const half_spacing = ils_.antenna_spacing_m * 0.5F;

    ils_wave_pipeline_.set_data(
        pos_wave_,
        ils::IlsWaveData{
            .start_position = { 0.0F, +half_spacing },
            .end_position   = world_pos,
        }
    );
    ils_wave_pipeline_.set_data(
        neg_wave_,
        ils::IlsWaveData{
            .start_position = { 0.0F, -half_spacing },
            .end_position   = world_pos,
        }
    );
}

} // namespace ltb
