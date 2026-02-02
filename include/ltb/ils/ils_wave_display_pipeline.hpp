// ///////////////////////////////////////////////////////////////////////////////////////
// A Logan Thomas Barnes project
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// project
#include "ltb/vlk/dd/mesh_data.hpp"
#include "ltb/vlk/dd/simple_mesh_2.hpp"
#include "ltb/vlk/objs/vulkan_graphics_pipeline.hpp"

namespace ltb::ils
{

struct IlsWavePipelineSettings
{
    uint32                         frame_count = 0U;
    vlk::objs::VulkanBuffer const& camera_ubo;
};

struct IlsWaveData
{
    glm::vec2 start_position       = { 0.0F, 0.0F };
    glm::vec2 end_position         = { 1.0F, 0.0F };
    float32   carrier_frequency_hz = 25.0F;

    glm::vec4 color      = glm::vec4( 1.0F );
    float32   line_width = 1.0F;
};

class IlsWaveDisplayPipeline
{
public:
    IlsWaveDisplayPipeline(
        vlk::objs::VulkanGpu&          gpu,
        vlk::objs::VulkanPresentation& presentation
    );

    auto initialize( IlsWavePipelineSettings const& settings ) -> utils::Result< void >;

    [[nodiscard( "Const getter" )]]
    auto is_initialized( ) const -> bool;

    auto initialize_wave( ) -> utils::Result< uint32 >;

    [[nodiscard( "Const getter" )]]
    auto get_data( uint32 id ) const -> IlsWaveData;
    auto set_data( uint32 id, IlsWaveData const& data ) -> void;

    auto draw( vlk::objs::FrameInfo const& frame ) -> utils::Result< void >;

private:
    vlk::objs::VulkanGpu&          gpu_;
    vlk::objs::VulkanPresentation& presentation_;

    vlk::objs::VulkanGraphicsPipeline pipeline_ = { gpu_, presentation_ };

    uint32                                    next_wave_id_     = 1U;
    std::vector< uint32 >                     ordered_wave_ids_ = { };
    std::unordered_map< uint32, IlsWaveData > wave_data_        = { };

    bool initialized_ = false;
};

} // namespace ltb::ils
