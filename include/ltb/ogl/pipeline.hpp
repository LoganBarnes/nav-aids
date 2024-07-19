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

struct FailedResult
{
    utils::Result<> result;

    template < typename Type >
    auto operator( )( Type& initializable ) -> bool
    {
        result = initializable.initialize( );
        return !result;
    }
};

// struct FailedResult
// {
//     utils::Result<> result;
//
//     template < typename Type >
//     auto operator( )( Type& type, std::string_view name ) -> bool
//     {
//         result = type.initialize( name );
//         return !result;
//     }
//
//     template < typename Type >
//     auto operator( )( Type& type ) -> bool
//     {
//         result = type.initialize( );
//         return !result;
//     }
//
//     template < GLenum shader_type >
//     auto operator( )( Shader< shader_type >& shader ) -> bool
//     {
//         result = shader.initialize( );
//         return !result;
//     }
// };

// template < typename... >
// struct Attributes
// {
// };
//
// template < typename... >
// struct Uniforms
// {
// };
//
// template < typename... NameTypes >
// struct AttributeNames
// {
//     std::tuple< NameTypes... > names;
// };
//
// template < typename... NameTypes >
// auto attribute_names( NameTypes&&... names )
// {
//     return AttributeNames{ .names = std::make_tuple( std::forward< NameTypes >( names )... ) };
// }
//
// template < typename... NameTypes >
// struct UniformNames
// {
//     std::tuple< NameTypes... > names;
// };
//
// template < typename... NameTypes >
// auto uniform_names( NameTypes&&... names )
// {
//     return UniformNames{ .names = std::make_tuple( std::forward< NameTypes >( names )... ) };
// }
//
// template < typename... AttributeTypes, size_t... Is, typename... AttributeNameTypes >
// auto make_attribute_tuple(
//     Program& program,
//     std::index_sequence< Is... >,
//     std::tuple< AttributeNameTypes... > const& attribute_names
// )
// {
//     return std::tuple< Attribute< AttributeTypes >... >{
//         Attribute< AttributeTypes >{ program, std::get< Is >( attribute_names ) }...
//     };
// }

//
// template < typename Attributes, typename Uniforms >
// class Pipeline;
//
// template < typename... AttributeTypes, typename... UniformTypes >
// class Pipeline< Attributes< AttributeTypes... >, Uniforms< UniformTypes... > >
// {
// public:
//     template < typename... AttributeNameTypes >
//     Pipeline( AttributeNames< AttributeNameTypes... > const& attribute_names )
//         : attributes{ make_attribute_tuple< AttributeTypes... >(
//               program,
//               std::index_sequence_for< AttributeTypes... >{ },
//               attribute_names.names
//           ) }
//     {
//     }
//
//     // ogl::Shader< GL_VERTEX_SHADER >   vertex_shader   = { };
//     // ogl::Shader< GL_FRAGMENT_SHADER > fragment_shader = { };
//     ogl::Program                      program         = { };
//
//     using Attributes = std::tuple< Attribute< AttributeTypes >... >;
//     using Uniforms   = std::tuple< Uniform< UniformTypes >... >;
//
//     Attributes attributes;
//     Uniforms   uniforms = { ogl::Uniform< UniformTypes >{ program }... };
//
//     ogl::Buffer      vertex_buffer = { };
//     ogl::VertexArray vertex_array  = { };
//
//     template < typename... UniformNameTypes >
//     auto initialize(
//         std::filesystem::path const&        vertex_shader_path,
//         std::filesystem::path const&        fragment_shader_path,
//         UniformNames< UniformNameTypes... > uniform_names
//     ) -> utils::Result<>
//     {
//         static_assert(
//             sizeof...( UniformTypes ) == sizeof...( UniformNameTypes ),
//             "Number of uniform types must match number of uniform names."
//         );
//
//         LTB_CHECK( vertex_shader.initialize( ) );
//         LTB_CHECK( fragment_shader.initialize( ) );
//         LTB_CHECK( program.initialize( ) );
//
//         LTB_CHECK( vertex_shader.load_and_compile( vertex_shader_path ) );
//         LTB_CHECK( fragment_shader.load_and_compile( fragment_shader_path ) );
//         LTB_CHECK( program.attach_and_link( vertex_shader, fragment_shader ) );
//
//         LTB_CHECK( initialize_tuple( std::index_sequence_for< AttributeTypes... >{ }, attributes )
//         );
//
//         LTB_CHECK( initialize_tuple(
//             std::index_sequence_for< UniformTypes... >{ },
//             uniforms,
//             uniform_names.names
//         ) );
//
//         vertex_buffer.initialize( );
//         vertex_array.initialize( );
//
//         return utils::success( );
//     }
//
//     auto destroy( ) -> void
//     {
//         vertex_array  = { };
//         vertex_buffer = { };
//
//         program         = { };
//         fragment_shader = { };
//         vertex_shader   = { };
//     }
//
// private:
//     template < size_t... Is, typename... Types, typename... NameTypes >
//     auto initialize_tuple(
//         std::index_sequence< Is... >,
//         std::tuple< Types... >&           types,
//         std::tuple< NameTypes... > const& names
//     ) -> utils::Result<>
//     {
//         if ( auto failed_result = FailedResult{ };
//              ( ( failed_result( std::get< Is >( types ), std::get< Is >( names ) ) ) || ... ) )
//         {
//             return failed_result.result;
//         }
//         return utils::success( );
//     }
//
//     template < size_t... Is, typename... Types >
//     auto initialize_tuple( std::index_sequence< Is... >, std::tuple< Types... >& types )
//         -> utils::Result<>
//     {
//         if ( auto failed_result = FailedResult{ };
//              ( ( failed_result( std::get< Is >( types ) ) ) || ... ) )
//         {
//             return failed_result.result;
//         }
//         return utils::success( );
//     }
// };

template < GLenum... shader_types >
auto initialize_program( Program& program, Shader< shader_types >&... shaders ) -> utils::Result<>
{
    if ( auto failed_result = FailedResult{ }; ( ( failed_result( shaders ) ) || ... ) )
    {
        return failed_result.result;
    }

    LTB_CHECK( program.initialize( ) );
    LTB_CHECK( program.attach_and_link( shaders... ) );

    return utils::success( );
}

} // namespace ltb::ogl
