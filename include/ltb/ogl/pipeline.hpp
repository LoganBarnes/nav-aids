#pragma once

// project
#include "ltb/ogl/buffer.hpp"
#include "ltb/ogl/program.hpp"
#include "ltb/ogl/shader.hpp"
#include "ltb/ogl/uniform.hpp"
#include "ltb/ogl/vertex_array.hpp"

// standard
#include <filesystem>
#include <tuple>

namespace ltb::ogl
{

struct FailedResult
{
    utils::Result<> result;

    template < typename UniformType >
    auto operator( )( ogl::Uniform< UniformType >& uniform, std::string const& name ) -> bool
    {
        result = uniform.initialize( name );
        return !result;
    }
};

template < typename... UniformTypes >
class Pipeline
{
public:
    ogl::Shader< GL_VERTEX_SHADER >   vertex_shader   = { };
    ogl::Shader< GL_FRAGMENT_SHADER > fragment_shader = { };
    ogl::Program                      program         = { };

    std::tuple< ogl::Uniform< UniformTypes >... > uniforms = {
        ogl::Uniform< UniformTypes >{ program }...,
    };

    ogl::Buffer      vertex_buffer = { };
    ogl::VertexArray vertex_array  = { };

    template < typename... UniformNames >
    auto initialize(
        std::filesystem::path const& vertex_shader_path,
        std::filesystem::path const& fragment_shader_path,
        UniformNames&&... uniform_names
    ) -> utils::Result<>
    {
        static_assert(
            sizeof...( UniformTypes ) == sizeof...( uniform_names ),
            "Number of uniform types must match number of uniform names."
        );

        LTB_CHECK( vertex_shader.initialize( ) );
        LTB_CHECK( fragment_shader.initialize( ) );
        LTB_CHECK( program.initialize( ) );

        LTB_CHECK( vertex_shader.load_and_compile( vertex_shader_path ) );
        LTB_CHECK( fragment_shader.load_and_compile( fragment_shader_path ) );
        LTB_CHECK( program.attach_and_link( vertex_shader, fragment_shader ) );

        // Initialize all uniforms and check for errors
        if ( auto failed_result = FailedResult{ };
             ( ( failed_result(
                   std::get< Uniform< UniformTypes > >( uniforms ),
                   std::forward< UniformNames >( uniform_names )
               ) )
               || ... ) )
        {
            return failed_result.result;
        }

        vertex_buffer.initialize( );
        vertex_array.initialize( );

        return utils::success( );
    }
};

} // namespace ltb::ogl
