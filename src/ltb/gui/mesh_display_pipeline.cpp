#include "ltb/gui/mesh_display_pipeline.hpp"

// project
#include "ltb/math/mesh.hpp"
#include "ltb/ogl/buffer.hpp"
#include "ltb/ogl/vertex_array.hpp"
#include "ltb/utils/initializable.hpp"

// standard
#include <algorithm>

namespace ltb::gui
{
namespace
{

template < glm::length_t L >
auto maybe_add_buffer(
    std::vector< ogl::Buffer >&                     vertex_buffers,
    ogl::Bound< ogl::VertexArray >&                 bound_vao,
    std::vector< glm::vec< L, float32 > > const&    data,
    ogl::Attribute< glm::vec< L, float32 > > const& attribute,
    std::string_view const                          name,
    size_t const                                    expected_size
) -> utils::Result< void >
{

    if ( !data.empty( ) )
    {
        if ( data.size( ) != expected_size )
        {
            return LTB_MAKE_UNEXPECTED_ERROR( "Mesh {} size does not match positions size", name );
        }
        auto bound_vbo = bind< GL_ARRAY_BUFFER >( vertex_buffers.emplace_back( ) );
        buffer_data( bound_vbo, data, GL_STATIC_DRAW );

        constexpr auto total_stride   = 0;
        constexpr auto attrib_divisor = 0;
        set_attributes(
            bound_vao,
            bound_vbo,
            { {
                .attribute_location      = attribute.location( ),
                .num_coordinates         = L,
                .data_type               = GL_FLOAT,
                .initial_offset_into_vbo = nullptr,
            } },
            total_stride,
            attrib_divisor
        );
    }
    return utils::success( );
}

auto mode_from_geometry_format( math::MeshFormat const format ) -> utils::Result< GLenum >
{
    switch ( format )
    {
        using enum math::MeshFormat;
        case Points:
            return static_cast< GLenum >( GL_POINTS );
        case Lines:
            return static_cast< GLenum >( GL_LINES );
        case LineStrip:
            return static_cast< GLenum >( GL_LINE_STRIP );
        case Triangles:
            return static_cast< GLenum >( GL_TRIANGLES );
        case TriangleStrip:
            return static_cast< GLenum >( GL_TRIANGLE_STRIP );
        case TriangleFan:
            return static_cast< GLenum >( GL_TRIANGLE_FAN );
    }
    return LTB_MAKE_UNEXPECTED_ERROR( "Unsupported MeshFormat" );
}

} // namespace

struct MeshDisplayPipeline::ShouldDisplay
{
    auto operator( )( InternalMeshData const& data ) const
    {
        return ( data.settings.visible ) && ( 0 != data.draw_count );
    }

    auto operator( )( std::pair< MeshId, InternalMeshData > const& data ) const
    {
        return operator( )( data.second );
    }
};

MeshDisplayPipeline::MeshDisplayPipeline( IncrementalIdGenerator< uint32 >& id_generator )
    : id_generator_( id_generator )
{
}

auto MeshDisplayPipeline::initialize( ) -> utils::Result< void >
{
    if ( is_initialized( ) )
    {
        return utils::success( );
    }

    LTB_CHECK(
        utils::initialize(
            vertex_shader_,
            fragment_shader_,
            program_,
            local_position_attribute_,
            local_normal_attribute_,
            local_uv_coords_attribute_,
            local_color_attribute_,
            clip_from_world_uniform_,
            world_from_local_uniform_,
            world_from_local_normals_uniform_,
            uniform_color_uniform_,
            color_mode_uniform_,
            shading_mode_uniform_
        )
    );

    initialized_ = true;

    return utils::success( );
}

auto MeshDisplayPipeline::is_initialized( ) const -> bool
{
    return initialized_;
}

auto MeshDisplayPipeline::initialize_mesh( math::Mesh3 const& mesh ) -> utils::Result< MeshId >
{
    if ( !is_initialized( ) )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "MeshDisplayPipeline not initialized" );
    }

    // auto mesh_data = InternalMeshData{ };
    // /// \todo Implement mesh data initialization
    // utils::ignore( mesh );
    //
    // auto const id = id_generator_.generate_id< MeshId >( );

    LTB_CHECK( auto const draw_mode, mode_from_geometry_format( mesh.format ) );

    // Create the OpenGL objects for the mesh.
    auto       mesh_data     = InternalMeshData{ .draw_mode = draw_mode };
    auto       bound_vao     = bind( mesh_data.vertex_array );
    auto const expected_size = mesh.positions.size( );

    LTB_CHECK( maybe_add_buffer(
        mesh_data.vertex_buffers,
        bound_vao,
        mesh.positions,
        local_position_attribute_,
        "positions",
        expected_size
    ) );
    LTB_CHECK( maybe_add_buffer(
        mesh_data.vertex_buffers,
        bound_vao,
        mesh.normals,
        local_normal_attribute_,
        "normals",
        expected_size
    ) );
    LTB_CHECK( maybe_add_buffer(
        mesh_data.vertex_buffers,
        bound_vao,
        mesh.uvs,
        local_uv_coords_attribute_,
        "uvs",
        expected_size
    ) );
    LTB_CHECK( maybe_add_buffer(
        mesh_data.vertex_buffers,
        bound_vao,
        mesh.vertex_colors,
        local_color_attribute_,
        "vertex_colors",
        expected_size
    ) );

    if ( !mesh.indices.empty( ) )
    {
        buffer_data(
            bind< GL_ELEMENT_ARRAY_BUFFER >( mesh_data.index_buffer.emplace( ) ),
            mesh.indices,
            GL_STATIC_DRAW
        );
        mesh_data.draw_count = static_cast< GLsizei >( mesh.indices.size( ) );
    }
    else
    {
        mesh_data.draw_count = static_cast< GLsizei >( mesh.positions.size( ) );
    }

    auto const id = id_generator_.generate_id< MeshId >( );

    // Check the buffer data is inserted successfully.
    LTB_CHECK_VALID( internal_data_.try_emplace( id, mesh_data ).second );

    return id;
}

auto MeshDisplayPipeline::update_settings( MeshId const id, MeshDisplaySettings const settings )
    -> void
{
    if ( !is_initialized( ) )
    {
        spdlog::error(
            "MeshDisplayPipeline::update_settings: "
            "MeshDisplayPipeline not initialized"
        );
        return;
    }
    if ( !internal_data_.contains( id ) )
    {
        spdlog::error(
            "MeshDisplayPipeline::update_settings: "
            "buffer with id {} not found",
            id.id( )
        );
        return;
    }

    auto& data = internal_data_.at( id );

    data.world_transform   = consolidate_transforms( settings.transforms );
    data.normals_transform = math::normals_transform( data.world_transform );

    data.settings = settings;
}

auto MeshDisplayPipeline::get_settings( MeshId const id ) const -> MeshDisplaySettings
{
    if ( !is_initialized( ) )
    {
        spdlog::error(
            "MeshDisplayPipeline::get_settings: "
            "MeshDisplayPipeline not initialized"
        );
        return { };
    }
    if ( !internal_data_.contains( id ) )
    {
        spdlog::error(
            "MeshDisplayPipeline::get_settings: "
            "buffer with id {} not found",
            id.id( )
        );
        return { };
    }

    return internal_data_.at( id ).settings;
}

auto MeshDisplayPipeline::remove( MeshId const id ) -> void
{
    internal_data_.erase( id );
}

auto MeshDisplayPipeline::clear( ) -> void
{
    internal_data_.clear( );
}

auto MeshDisplayPipeline::draw( math::cam::CameraRenderParams const& cam ) -> void
{
    if ( !initialized_ )
    {
        spdlog::error( "MeshDisplayPipeline not initialized" );
        return;
    }

    // Not an error but still an early return.
    if ( !std::ranges::any_of( internal_data_, ShouldDisplay{ } ) )
    {
        // No buffers to render
        return;
    }

    auto bound_program = bind( program_ );
    set( clip_from_world_uniform_, cam.clip_from_world );

    for ( auto const& [ id, data ] : internal_data_ )
    {
        utils::ignore( id );

        if ( ShouldDisplay{ }( data ) )
        {
            set( world_from_local_uniform_, data.world_transform );
            set( world_from_local_normals_uniform_, data.normals_transform );
            set( uniform_color_uniform_, data.settings.custom_color );
            set( color_mode_uniform_, std::to_underlying( data.settings.color_mode ) );
            set( shading_mode_uniform_, std::to_underlying( data.settings.shading_mode ) );

            if ( data.index_buffer.has_value( ) )
            {
                constexpr uint32 const* index_offset = nullptr;
                ogl::draw(
                    bound_program,
                    bind( data.vertex_array ),
                    bind< GL_ELEMENT_ARRAY_BUFFER >( data.index_buffer.value( ) ),
                    data.draw_mode,
                    index_offset,
                    data.draw_count
                );
            }
            else
            {
                constexpr auto start = GLsizei{ 0 };
                ogl::draw(
                    bound_program,
                    bind( data.vertex_array ),
                    data.draw_mode,
                    start,
                    data.draw_count
                );
            }
        }
    }
}

} // namespace ltb::gui
