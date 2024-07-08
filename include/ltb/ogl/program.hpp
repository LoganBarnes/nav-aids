#pragma once

// project
#include "ltb/ogl/shader.hpp"
#include "ltb/ogl/texture.hpp"
#include "ltb/ogl/type_traits.hpp"
#include "ltb/ogl/uniform_setter.hpp"
#include "ltb/ogl/utils.hpp"
#include "ltb/ogl/vertex_array.hpp"
#include "ltb/utils/types.hpp"

// external
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// standard
#include <filesystem>
#include <memory>
#include <optional>

namespace ltb::ogl
{

struct ProgramData
{
    /// \brief OpenGL ID from `glCreateProgram()`
    GLuint gl_id = 0U;
};

class Program
{
public:
    Program( ) = default;

    /// \brief Initialize the program object. This must
    ///        be called before using the program.
    auto initialize( ) -> utils::Result<>;

    /// \brief The raw settings stored for this program.
    [[nodiscard( "Const getter" )]]
    auto data( ) const -> ProgramData const&;

    /// \brief Attach and link the given shaders to the program.
    template < GLenum... ShaderTypes >
    auto attach_and_link( Shader< ShaderTypes > const&... shaders ) const -> utils::Result<>;

    /// \brief Get the shader attribute location.
    [[nodiscard( "Const getter" )]]
    auto get_attribute_location( std::string const& name ) const -> utils::Result< GLuint >;

    /// \brief Set a scalar or vector uniform.
    /// \tparam ValueType is the uniform base type: bool, int, uint, float, or double
    /// \param name is the name of the uniform in the shader
    /// \param value is a pointer to the data being passed to the shader
    /// \param size is the element size (T == 1, tvec2 == 2, tvec3 == 3, tvec4 == 4)
    /// \param count is the number of elements (1 unless using an array)
    template < typename ValueType >
    auto set_scalar_or_vec_uniform(
        std::string const& name,
        ValueType const*   value,
        int32              size,
        int32              count
    ) const -> utils::Result< Program const* >;

    /// \brief Set a boolean scalar or vector uniform.
    /// \param name is the name of the uniform in the shader
    /// \param value is a pointer to the bool data being passed to the shader
    /// \param size is the element size (T == 1, bvec2 == 2, bvec3 == 3, bvec4 == 4)
    /// \param count is the number of elements (1 unless using an array)
    auto
    set_scalar_or_vec_uniform( std::string const& name, bool const* value, int32 size, int32 count )
        const -> utils::Result< Program const* >;

    /// \brief Sets a matrix uniform
    /// \tparam T is the matrix type: float or double
    /// \param name is the name of the uniform in the shader
    /// \param value is a pointer to the data being passed to the shader
    /// \param size is the element size (mat2 == 2, mat3 == 3, mat4 == 4), default = 4
    /// \param count is the number of elements (1 unless using an array), default = 1
    template < typename T >
    auto set_matrix_uniform( std::string const& name, T const* value, int32 size, int32 count )
        const -> utils::Result< Program const* >;

    template < typename T >
    auto
    set_uniform( std::string const& name, T const& value ) const -> utils::Result< Program const* >
    {
        return ogl::UniformSetter< T >{ }( *this, name, value );
        //     ^^^^^^^^^^^^^^^^^^^^^ Specialize this struct to support custom types
    }

    /// \brief Set a buffer uniform.
    [[nodiscard( "Check errors" )]]
    auto set_uniform(
        std::string const&                               name,
        Bound< Buffer, GL_SHADER_STORAGE_BUFFER > const& buffer,
        GLuint                                           binding,
        GLintptr                                         byte_offset,
        GLsizeiptr                                       size_in_bytes
    ) const -> utils::Result< Program const* >;

    /// \brief Set a texture uniform.
    template < GLenum bind_type >
    auto set_uniform(
        std::string const&                 name,
        Bound< Texture, bind_type > const& bound_texture,
        GLint                              active_tex
    ) const -> utils::Result< Program const* >;

    static auto static_bind( Program const& program ) -> void;
    static auto static_bind( GLuint raw_gl_id ) -> void;

private:
    ProgramData             data_    = { };
    std::shared_ptr< void > deleter_ = nullptr;
};

template < GLenum... ShaderTypes >
auto Program::attach_and_link( Shader< ShaderTypes > const&... shaders ) const -> utils::Result<>
{
    auto const id = data( ).gl_id;

    // Attach all shaders to program.
    ( glAttachShader( id, shaders.data( ).gl_id ), ... );

    // Attempt to link program.
    glLinkProgram( id );

    // Shaders are no longer needed after linking.
    ( glDetachShader( id, shaders.data( ).gl_id ), ... );

    // Check program for linking errors.
    auto result = GLint{ GL_FALSE };
    glGetProgramiv( id, GL_LINK_STATUS, &result );

    if ( GL_FALSE == result )
    {
        auto log_length = GLint{ 0 };
        glGetProgramiv( id, GL_INFO_LOG_LENGTH, &log_length );

        auto gl_error = std::vector< char >( static_cast< size_t >( log_length ) );
        glGetProgramInfoLog( id, log_length, nullptr, gl_error.data( ) );

        return LTB_MAKE_UNEXPECTED_ERROR( "Program: {}", gl_error.data( ) );
    }

    return utils::success( );
}

template < GLenum bind_type >
auto Program::set_uniform(
    std::string const&                 name,
    Bound< Texture, bind_type > const& bound_texture,
    GLint const                        active_tex
) const -> utils::Result< Program const* >
{
    // This argument is only provided to ensure the texture
    // is bound, but nothing has to be done with it.
    utils::ignore( bound_texture );

    auto const location = glGetUniformLocation( data( ).gl_id, name.c_str( ) );
    if ( location < 0 )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "Uniform '{}' not found in program.", name );
    }

    glActiveTexture( static_cast< GLenum >( GL_TEXTURE0 + active_tex ) );
    glProgramUniform1i( data( ).gl_id, location, active_tex );

    return this;
}

template < glm::length_t L, typename T >
struct UniformSetter< glm::vec< L, T > >
{
    auto
    operator( )( Program const& program, std::string const& name, glm::vec< L, T > const& value )
        const -> utils::Result< Program const* >
    {
        return program.set_scalar_or_vec_uniform< T >( name, glm::value_ptr( value ), L, 1 );
    }
};

template < glm::length_t L, typename T >
struct UniformSetter< glm::mat< L, L, T > >
{
    auto
    operator( )( Program const& program, std::string const& name, glm::mat< L, L, T > const& value )
        const -> utils::Result< Program const* >
    {
        return program.set_matrix_uniform< T >( name, glm::value_ptr( value ), L, 1 );
    }
};

template < typename IndexType >
constexpr auto is_index_type_v = is_any_same_v< IndexType, GLubyte, GLushort, GLuint >;

auto draw(
    Bound< Program > const&     bound_program,
    Bound< VertexArray > const& bound_vertex_array,
    GLenum                      mode,
    GLsizei                     start,
    GLsizei                     num_elements
) -> void;

template < typename IndexType >
    requires is_index_type_v< IndexType >
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
    requires is_index_type_v< IndexType >
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
