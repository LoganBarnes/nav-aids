// ///////////////////////////////////////////////////////////////////////////////////////
// A Logan Thomas Barnes project
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "ltb/cam/camera_2d.hpp"
#include "ltb/exec/update_loop.hpp"
#include "ltb/gui/imgui_glfw_vulkan_setup.hpp"
#include "ltb/vlk/dd/lines_pipeline_2.hpp"
#include "ltb/vlk/objs/vulkan_buffer.hpp"
#include "ltb/vlk/objs/vulkan_command_and_sync.hpp"
#include "ltb/vlk/objs/vulkan_gpu.hpp"

// standard
#include <unordered_set>

namespace ltb
{

struct IlsParams
{
    int32     antenna_pairs_     = 1;
    float32   antenna_spacing_m_ = 5.0F;
    glm::vec3 output_channels_   = { 1.0F, 1.0F, 1.0F };
    float32   output_scale_      = 0.1F;
};

class IlsApp
{
public:
    explicit IlsApp( window::GlfwContext& glfw_context, window::GlfwWindow& glfw_window );

    auto initialize( ) -> utils::Result< exec::UpdateLoopStatus >;

    [[nodiscard( "Const getter" )]]
    auto is_initialized( ) const -> bool;

    auto fixed_step_update( exec::UpdateLoopStatus const& status ) -> exec::UpdateRequests;
    auto frame_update( exec::UpdateLoopStatus const& status ) -> exec::UpdateRequests;

    auto configure_gui( ) -> void;

    auto on_resize( glm::ivec2 size ) -> utils::Result< void >;
    auto clean_up( ) -> utils::Result< void >;

private:
    window::GlfwContext& glfw_context_;
    window::GlfwWindow&  glfw_window_;

    vlk::objs::VulkanGpu gpu_                        = { glfw_context_, glfw_window_ };
    vk::Queue            graphics_and_compute_queue_ = nullptr;
    vk::Queue            presentation_queue_         = nullptr;

    vlk::objs::VulkanPresentation presentation_ = { gpu_ };
    gui::ImguiGlfwVulkanSetup     imgui_        = { glfw_window_, gpu_, presentation_ };

    IlsParams ils_ = { };

    vlk::objs::VulkanBuffer      camera_ubo_            = { gpu_ };
    cam::Camera2d                camera_                = { };
    std::unordered_set< uint32 > camera_frames_updated_ = { };

    vlk::dd::LinesPipeline2         line_display_          = { gpu_, presentation_ };
    vlk::objs::VulkanCommandAndSync graphics_cmd_and_sync_ = { gpu_ };

    vlk::dd::SimpleMeshUniforms* conversion_line_ = nullptr;

    bool initialized_ = false;

    auto initialize_gpu( ) -> utils::Result< IlsApp* >;
    auto initialize_presentation( ) -> utils::Result< IlsApp* >;
    auto initialize_fluid( ) -> utils::Result< IlsApp* >;
    auto initialize_camera( ) -> utils::Result< IlsApp* >;
    auto initialize_display_pipeline( ) -> utils::Result< IlsApp* >;
    auto initialize_meshes( ) -> utils::Result< IlsApp* >;

    auto compute( ) -> utils::Result< void >;

    auto render( ) -> utils::Result< void >;
    auto update_camera_uniforms( vlk::objs::FrameInfo const& frame ) -> utils::Result< void >;
    auto record_render_commands( vlk::objs::FrameInfo const& frame ) -> utils::Result< void >;
};

} // namespace ltb
