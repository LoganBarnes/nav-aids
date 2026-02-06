// ///////////////////////////////////////////////////////////////////////////////////////
// A Logan Thomas Barnes project
// ///////////////////////////////////////////////////////////////////////////////////////
#include "ltb/ils/ils_wave_display_pipeline.hpp"

// project
#include "ltb/nav/ltb_nav_config.hpp"
#include "ltb/vlk/buffer_utils.hpp"
#include "ltb/vlk/device_memory_utils.hpp"
#include "ltb/vlk/objs/frame_info.hpp"

// external
#include "spdlog/fmt/bundled/os.h"
#include <range/v3/range/conversion.hpp>
#include <spdlog/spdlog.h>

namespace ltb::ils
{
namespace
{

constexpr auto display_uniform_offset = 48U;

enum class Antenna : uint32
{
    Both,
    Pos,
    Neg,
};

struct IlsWaveDataUniforms
{
    glm::vec2 pos_start            = { 0.0F, +1.0F };
    glm::vec2 neg_start            = { 0.0F, -1.0F };
    glm::vec2 end                  = { 1.0F, 0.0F };
    float32   carrier_frequency_hz = 25.0F;
    uint32    line_segments        = 1U;
    Antenna   antenna              = Antenna::Both;
};

struct IlsWaveDisplayUniforms
{
    glm::vec4   color      = glm::vec4( 1.0F );
    float32     line_width = 1.0F;
    IlsWaveForm wave_form  = IlsWaveForm::Combined;
};

} // namespace

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
        {
            .spirv_file = nav::config::ils_shader_dir_path( ) / "ils_wave.tesc.spv",
            .stage      = vk::ShaderStageFlagBits::eTessellationControl,
        },
        {
            .spirv_file = nav::config::ils_shader_dir_path( ) / "ils_wave.tese.spv",
            .stage      = vk::ShaderStageFlagBits::eTessellationEvaluation,
        },
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
            .setStageFlags( vk::ShaderStageFlagBits::eTessellationEvaluation ),
    };

    auto uniform_push_constants = std::vector{
        // simulation uniforms
        vk::PushConstantRange{ }
            .setStageFlags( vk::ShaderStageFlagBits::eVertex )
            .setOffset( 0U )
            .setSize( sizeof( IlsWaveDataUniforms ) ),
        vk::PushConstantRange{ }
            .setStageFlags( vk::ShaderStageFlagBits::eTessellationEvaluation )
            .setOffset( display_uniform_offset )
            .setSize( sizeof( IlsWaveDisplayUniforms ) ),
    };

    constexpr auto tessellation_state
        = vk::PipelineTessellationStateCreateInfo{ }.setPatchControlPoints( 1U );

    LTB_CHECK( pipeline_.initialize( {
        .shader_modules         = std::move( shader_modules ),
        .descriptor_set_count   = settings.frame_count,
        .uniform_binding_sets   = { std::move( uniform_bindings ) },
        .uniform_push_constants = std::move( uniform_push_constants ),

        .pipeline = {
            .primitive_topology = vk::PrimitiveTopology::ePatchList,
            .depth_stencil      = std::nullopt,
            .tessellation_state = tessellation_state,
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
    auto const wave_id = next_wave_id_;
    ++next_wave_id_;
    ordered_wave_ids_.push_back( wave_id );
    utils::ignore( wave_data_.emplace( wave_id, IlsWaveData{ } ) );
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

        auto const line_length = glm::distance( wave_data.start_position, wave_data.end_position );
        auto const num_patches = std::ceil( line_length * wave_data.carrier_frequency_hz );
        auto const line_segments = static_cast< uint32 >( std::max( 1.0F, num_patches ) );

        auto const data_uniforms = IlsWaveDataUniforms{
            .pos_start            = ( wave_data.start_position ),
            .neg_start            = ( wave_data.start_position ),
            .end                  = wave_data.end_position,
            .carrier_frequency_hz = wave_data.carrier_frequency_hz,
            .line_segments        = line_segments,
            .antenna = ( wave_data.start_position.y > 0.0F ? Antenna::Pos : Antenna::Neg ),
        };

        constexpr auto data_uniform_offset = 0U;
        frame.command_buffer.pushConstants(
            pipeline_.pipeline_layout( ).get( ),
            vk::ShaderStageFlagBits::eVertex,
            data_uniform_offset,
            sizeof( data_uniforms ),
            &data_uniforms
        );

        auto const display_uniforms = IlsWaveDisplayUniforms{
            .color      = wave_data.color,
            .line_width = wave_data.line_width,
            .wave_form  = wave_data.wave_form,
        };

        frame.command_buffer.pushConstants(
            pipeline_.pipeline_layout( ).get( ),
            vk::ShaderStageFlagBits::eTessellationEvaluation,
            display_uniform_offset,
            sizeof( display_uniforms ),
            &display_uniforms
        );

        constexpr auto instance_count = 1U;
        constexpr auto first_vertex   = 0U;
        constexpr auto first_instance = 0U;
        frame.command_buffer.draw( line_segments, instance_count, first_vertex, first_instance );
    }

    return utils::success( );
}

} // namespace ltb::ils
