#pragma once

// project
#include "ltb/ogl/shader.hpp"
#include "ltb/ogl/type_traits.hpp"
#include "ltb/ogl/utils.hpp"
#include "ltb/utils/types.hpp"

// standard
#include <filesystem>
#include <memory>
#include <optional>
#include <variant>

namespace ltb::ogl
{

struct ProgramData
{
    /// \brief OpenGL ID from `glCreateProgram()`
    GLuint gl_id = 0U;
};

using AnyShader = std::variant<
#if defined( GL_COMPUTE_SHADER )
    std::reference_wrapper< Shader< GL_COMPUTE_SHADER > const >,
#endif
    std::reference_wrapper< Shader< GL_VERTEX_SHADER > const >,
    std::reference_wrapper< Shader< GL_TESS_CONTROL_SHADER > const >,
    std::reference_wrapper< Shader< GL_TESS_EVALUATION_SHADER > const >,
    std::reference_wrapper< Shader< GL_GEOMETRY_SHADER > const >,
    std::reference_wrapper< Shader< GL_FRAGMENT_SHADER > const > >;

class Program
{
public:
    template < GLenum... ShaderTypes >
    // NOLINTNEXTLINE(google-explicit-constructor)
    explicit( false ) Program( Shader< ShaderTypes > const&... shaders );

    /// \brief Initialize the program object. This must
    ///        be called before using the program.
    auto initialize( ) -> utils::Result<>;

    /// \brief Check if the program has been successfully initialized.
    [[nodiscard( "Const getter" )]]
    auto is_initialized( ) const -> bool;

    /// \brief The raw settings stored for this program.
    [[nodiscard( "Const getter" )]]
    auto data( ) const -> ProgramData const&;

    static auto static_bind( Program const& program ) -> void;
    static auto static_bind( GLuint raw_gl_id ) -> void;

private:
    std::vector< AnyShader > shaders_ = { };
    ProgramData              data_    = { };
    std::shared_ptr< void >  deleter_ = nullptr;
};

template < GLenum... ShaderTypes >
Program::Program( Shader< ShaderTypes > const&... shaders )
    : shaders_( { std::cref( shaders )... } )
{
}

template < typename IndexType >
concept IsIndexType = IsAny< IndexType, GLubyte, GLushort, GLuint >;

auto draw(
    Bound< Program > const&     bound_program,
    Bound< VertexArray > const& bound_vertex_array,
    GLenum                      mode,
    GLsizei                     start,
    GLsizei                     num_elements
) -> void;

template < typename IndexType >
    requires IsIndexType< IndexType >
auto draw(
    Bound< Program > const&                         bound_program,
    Bound< VertexArray > const&                     bound_vertex_array,
    Bound< Buffer, GL_ELEMENT_ARRAY_BUFFER > const& bound_index_buffer,
    GLenum                                          mode,
    IndexType const*                                index_offset,
    GLsizei                                         num_elements
) -> void;

auto draw_instanced(
    Bound< Program > const&     bound_program,
    Bound< VertexArray > const& bound_vertex_array,
    GLenum                      mode,
    GLsizei                     start,
    GLsizei                     num_elements,
    GLsizei                     instance_count
) -> void;

template < typename IndexType >
    requires IsIndexType< IndexType >
auto draw_instanced(
    Bound< Program > const&                         bound_program,
    Bound< VertexArray > const&                     bound_vertex_array,
    Bound< Buffer, GL_ELEMENT_ARRAY_BUFFER > const& bound_index_buffer,
    GLenum                                          mode,
    IndexType const*                                index_offset,
    GLsizei                                         num_elements,
    GLsizei                                         instance_count
) -> void;

} // namespace ltb::ogl
