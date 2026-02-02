// ///////////////////////////////////////////////////////////////////////////////////////
// A Logan Thomas Barnes project
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/ils/ils_wave_display_pipeline.hpp"

// project
#include "ltb/nav/ltb_nav_config.hpp"
#include "ltb/vlk/buffer_utils.hpp"
#include "ltb/vlk/check.hpp"
#include "ltb/vlk/device_memory_utils.hpp"
#include "ltb/vlk/objs/frame_info.hpp"

// external
#include "spdlog/fmt/bundled/os.h"
#include <range/v3/range/conversion.hpp>
#include <spdlog/spdlog.h>

namespace ltb::ils
{

IlsWaveDisplayPipeline::IlsWaveDisplayPipeline(
    vlk::objs::VulkanGpu&          gpu,
    vlk::objs::VulkanPresentation& presentation
)
    : gpu_( gpu )
    , presentation_( presentation )
{
}

auto IlsWaveDisplayPipeline::initialize( IlsWavePipelineSettings const& settings )
    -> utils::Result< void >
{
    if ( this->is_initialized( ) )
    {
        return utils::success( );
    }
    LTB_CHECK_VALID( gpu_.is_initialized( ) );
    LTB_CHECK_VALID( presentation_.is_initialized( ) );
    LTB_CHECK_VALID( settings.frame_count > 0U );
    LTB_CHECK_VALID( settings.camera_ubo.is_initialized( ) );

    auto shader_modules = std::vector< vlk::ShaderModuleSettings >{
        {
            .spirv_file = nav::config::ils_shader_dir_path( ) / "ils_wave.vert.spv",
            .stage      = vk::ShaderStageFlagBits::eVertex,
        },
        // {
        //     .spirv_file = nav::config::ils_shader_dir_path( ) / "ils_wave.tesc.spv",
        //     .stage      = vk::ShaderStageFlagBits::eTessellationControl,
        // },
        // {
        //     .spirv_file = nav::config::ils_shader_dir_path( ) / "ils_wave.tese.spv",
        //     .stage      = vk::ShaderStageFlagBits::eTessellationEvaluation,
        // },
        {
            .spirv_file = nav::config::ils_shader_dir_path( ) / "ils_wave.frag.spv",
            .stage      = vk::ShaderStageFlagBits::eFragment,
        },
    };

    auto uniform_bindings = std::vector{
        // camera UBO
        vk::DescriptorSetLayoutBinding{ }
            .setBinding( 0U )
            .setDescriptorType( vk::DescriptorType::eUniformBuffer )
            .setDescriptorCount( 1U )
            .setStageFlags( vk::ShaderStageFlagBits::eVertex ),
        // .setStageFlags( vk::ShaderStageFlagBits::eTessellationEvaluation ),
    };

    auto uniform_push_constants = std::vector{
        // simulation uniforms
        vk::PushConstantRange{ }
            .setStageFlags( vk::ShaderStageFlagBits::eVertex )
            // .setStageFlags( vk::ShaderStageFlagBits::eTessellationEvaluation )
            .setOffset( 0U )
            .setSize( sizeof( IlsWaveData ) ),
    };

    auto vertex_bindings = std::vector< vk::VertexInputBindingDescription >{
        // vk::VertexInputBindingDescription{ }
        //     .setBinding( 0U )
        //     .setStride( sizeof( float32 ) )
        //     .setInputRate( vk::VertexInputRate::eVertex ),
        // vk::VertexInputBindingDescription{ }
        //     .setBinding( 1U )
        //     .setStride( sizeof( float32 ) )
        //     .setInputRate( vk::VertexInputRate::eVertex ),
    };

    auto vertex_attributes = std::vector< vk::VertexInputAttributeDescription >{
        // vk::VertexInputAttributeDescription{ }
        //     .setBinding( 0U )
        //     .setLocation( 0U )
        //     .setFormat( vk::Format::eR32Sfloat )
        //     .setOffset( 0U ),
        // vk::VertexInputAttributeDescription{ }
        //     .setBinding( 1U )
        //     .setLocation( 1U )
        //     .setFormat( vk::Format::eR32Sfloat )
        //     .setOffset( 0U ),
    };

    // constexpr auto tessellation_state
    // = vk::PipelineTessellationStateCreateInfo{ }.setPatchControlPoints( 1U );

    LTB_CHECK( pipeline_.initialize( {
        .shader_modules         = std::move( shader_modules ),
        .descriptor_set_count   = settings.frame_count,
        .uniform_binding_sets   = { std::move( uniform_bindings ) },
        .uniform_push_constants = std::move( uniform_push_constants ),

        .pipeline = {
            .vertex_bindings   = std::move( vertex_bindings ),
            .vertex_attributes = std::move( vertex_attributes ),
            // .primitive_topology = vk::PrimitiveTopology::ePatchList,
            .primitive_topology = vk::PrimitiveTopology::eLineList,
            .depth_stencil      = std::nullopt,
            // .tessellation_state = tessellation_state,
            .tessellation_state = std::nullopt,
        },
    } ) );

    LTB_CHECK_VALID( 1UZ == pipeline_.descriptor_sets( ).size( ) );

    auto const& uniform_descriptor_sets = pipeline_.descriptor_sets( ).front( ).get( );
    for ( auto frame_index = 0U; frame_index < settings.frame_count; ++frame_index )
    {
        LTB_CHECK_VALID( frame_index < settings.camera_ubo.layout( ).ranges.size( ) );
        LTB_CHECK_VALID( frame_index < uniform_descriptor_sets.size( ) );

        auto const& ubo_memory_range = settings.camera_ubo.layout( ).ranges[ frame_index ];
        auto const& descriptor_set   = uniform_descriptor_sets[ frame_index ];

        auto const ubo_info = vk::DescriptorBufferInfo{ }
                                  .setBuffer( settings.camera_ubo.buffer( ).get( ) )
                                  .setOffset( ubo_memory_range.offset )
                                  .setRange( ubo_memory_range.size );

        auto const descriptor_writes = std::vector{
            vk::WriteDescriptorSet{ }
                .setDstSet( descriptor_set )
                .setDstBinding( 0U )
                .setDstArrayElement( 0U )
                .setDescriptorType( vk::DescriptorType::eUniformBuffer )
                .setBufferInfo( ubo_info ),
        };

        gpu_.device( ).get( ).updateDescriptorSets( descriptor_writes, { } );
    }

    initialized_ = true;

    return utils::success( );
}

auto IlsWaveDisplayPipeline::is_initialized( ) const -> bool
{
    return initialized_;
}

auto IlsWaveDisplayPipeline::initialize_wave( ) -> utils::Result< uint32 >
{
    auto const wave_id = next_wave_id_++;
    ordered_wave_ids_.push_back( wave_id );
    wave_data_.emplace( wave_id, IlsWaveData{ } );
    return wave_id;
}

auto IlsWaveDisplayPipeline::get_data( uint32 const id ) const -> IlsWaveData
{
    if ( !wave_data_.contains( id ) )
    {
        spdlog::error( "IlsWaveDisplayPipeline::get_data: Invalid wave ID {}", id );
        return { };
    }
    return wave_data_.at( id );
}

auto IlsWaveDisplayPipeline::set_data( uint32 const id, IlsWaveData const& data ) -> void
{
    if ( !wave_data_.contains( id ) )
    {
        spdlog::error( "IlsWaveDisplayPipeline::set_data: Invalid wave ID {}", id );
        return;
    }
    wave_data_.at( id ) = data;
}

auto IlsWaveDisplayPipeline::draw( vlk::objs::FrameInfo const& frame ) -> utils::Result< void >
{
    if ( ordered_wave_ids_.empty( ) )
    {
        return utils::success( );
    }

    pipeline_.bind( frame.command_buffer );

    LTB_CHECK( pipeline_.bind_descriptor_sets( frame ) );

    for ( auto const& wave_id : ordered_wave_ids_ )
    {
        auto const wave_data = this->get_data( wave_id );

        constexpr auto model_offset = 0U;
        frame.command_buffer.pushConstants(
            pipeline_.pipeline_layout( ).get( ),
            // vk::ShaderStageFlagBits::eTessellationEvaluation,
            vk::ShaderStageFlagBits::eVertex,
            model_offset,
            sizeof( wave_data ),
            &wave_data
        );

        constexpr auto vertex_count   = 2U;
        constexpr auto instance_count = 1U;
        constexpr auto first_vertex   = 0U;
        constexpr auto first_instance = 0U;
        frame.command_buffer.draw( vertex_count, instance_count, first_vertex, first_instance );
    }

    return utils::success( );
}

} // namespace ltb::ils
