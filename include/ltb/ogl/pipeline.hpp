#pragma once

// project
#include "ltb/ogl/buffer.hpp"
#include "ltb/ogl/program.hpp"
#include "ltb/ogl/program_attribute.hpp"
#include "ltb/ogl/program_uniform.hpp"
#include "ltb/ogl/shader.hpp"
#include "ltb/ogl/vertex_array.hpp"

// standard
#include <filesystem>
#include <tuple>

namespace ltb::ogl
{

template < typename... >
struct Attributes
{
};

template < typename... >
struct Uniforms
{
};

template < typename... NameTypes >
struct AttributeNames
{
    std::tuple< NameTypes... > names;
};

template < typename... NameTypes >
auto attribute_names( NameTypes&&... names )
{
    return AttributeNames{ .names = std::make_tuple( std::forward< NameTypes >( names )... ) };
}

template < typename... NameTypes >
struct UniformNames
{
    std::tuple< NameTypes... > names;
};

template < typename... NameTypes >
auto uniform_names( NameTypes&&... names )
{
    return UniformNames{ .names = std::make_tuple( std::forward< NameTypes >( names )... ) };
}

template < typename Attributes, typename Uniforms >
class Pipeline;

template < typename... AttributeTypes, typename... UniformTypes >
class Pipeline< Attributes< AttributeTypes... >, Uniforms< UniformTypes... > >
{
public:
    ogl::Shader< GL_VERTEX_SHADER >   vertex_shader   = { };
    ogl::Shader< GL_FRAGMENT_SHADER > fragment_shader = { };
    ogl::Program                      program         = { };

    std::tuple< Attribute< AttributeTypes >... > attributes = {
        ogl::Attribute< AttributeTypes >{ program }...,
    };

    std::tuple< Uniform< UniformTypes >... > uniforms = {
        ogl::Uniform< UniformTypes >{ program }...,
    };

    ogl::Buffer      vertex_buffer = { };
    ogl::VertexArray vertex_array  = { };

    template < typename... AttributeNameTypes, typename... UniformNameTypes >
    auto initialize(
        std::filesystem::path const&            vertex_shader_path,
        std::filesystem::path const&            fragment_shader_path,
        AttributeNames< AttributeNameTypes... > attribute_names,
        UniformNames< UniformNameTypes... >     uniform_names
    ) -> utils::Result<>
    {
        static_assert(
            sizeof...( AttributeTypes ) == sizeof...( AttributeNameTypes ),
            "Number of attribute types must match number of attribute names."
        );

        static_assert(
            sizeof...( UniformTypes ) == sizeof...( UniformNameTypes ),
            "Number of uniform types must match number of uniform names."
        );

        LTB_CHECK( vertex_shader.initialize( ) );
        LTB_CHECK( fragment_shader.initialize( ) );
        LTB_CHECK( program.initialize( ) );

        LTB_CHECK( vertex_shader.load_and_compile( vertex_shader_path ) );
        LTB_CHECK( fragment_shader.load_and_compile( fragment_shader_path ) );
        LTB_CHECK( program.attach_and_link( vertex_shader, fragment_shader ) );

        LTB_CHECK( initialize_tuple(
            std::index_sequence_for< AttributeTypes... >{ },
            attributes,
            attribute_names.names
        ) );

        LTB_CHECK( initialize_tuple(
            std::index_sequence_for< UniformTypes... >{ },
            uniforms,
            uniform_names.names
        ) );

        vertex_buffer.initialize( );
        vertex_array.initialize( );

        return utils::success( );
    }

    auto destroy( ) -> void
    {
        vertex_array  = { };
        vertex_buffer = { };

        program         = { };
        fragment_shader = { };
        vertex_shader   = { };
    }

private:
    struct FailedResult
    {
        utils::Result<> result;

        template < typename Type >
        auto operator( )( Type& type, std::string_view name ) -> bool
        {
            result = type.initialize( name );
            return !result;
        }
    };

    template < size_t... Is, typename... Types, typename... NameTypes >
    auto initialize_tuple(
        std::index_sequence< Is... >,
        std::tuple< Types... >&           types,
        std::tuple< NameTypes... > const& names
    ) -> utils::Result<>
    {
        if ( auto failed_result = FailedResult{ };
             ( ( failed_result( std::get< Is >( types ), std::get< Is >( names ) ) ) || ... ) )
        {
            return failed_result.result;
        }
        return utils::success( );
    }
};

} // namespace ltb::ogl
