#pragma once

// project
#include "ltb/ogl/fwd.hpp"
#include "ltb/ogl/type_traits.hpp"
#include "ltb/utils/result.hpp"

// standard
#include <filesystem>
#include <memory>

namespace ltb::ogl
{

template < GLenum shader_type >
concept IsShaderType = IsAnyOf<
    shader_type,
#if defined( GL_COMPUTE_SHADER )
    GL_COMPUTE_SHADER,
#endif
    GL_VERTEX_SHADER,
    GL_TESS_CONTROL_SHADER,
    GL_TESS_EVALUATION_SHADER,
    GL_GEOMETRY_SHADER,
    GL_FRAGMENT_SHADER >;

/// \brief Blah
struct ShaderData
{
    /// \brief The OpenGL ID from `glCreateShader()`
    GLuint gl_id = 0U;
};

template < GLenum shader_type >
    requires IsShaderType< shader_type >
class Shader
{
public:
    // NOLINTNEXTLINE(google-explicit-constructor)
    explicit( false ) Shader( std::filesystem::path filename );

    /// \brief Initialize the shader object. This must
    ///        be called before using the shader.
    auto initialize( ) -> utils::Result<>;

    /// \brief Returns whether the shader has been successfully initialized.
    [[nodiscard( "Const getter" )]]
    auto is_initialized( ) const -> bool;

    /// \brief The raw settings stored for this shader.
    [[nodiscard( "Const getter" )]]
    auto data( ) const -> ShaderData const&;

private:
    std::filesystem::path   filename_;
    ShaderData              data_    = { };
    std::shared_ptr< void > deleter_ = nullptr;

    auto create_shader( ) -> utils::Result< Shader* >;
    auto load_and_compile( ) -> utils::Result< Shader* >;
};

template < GLenum shader_type >
    requires IsShaderType< shader_type >
[[nodiscard( "Use string" )]]
constexpr auto to_string( ) -> char const*
{
    switch ( shader_type )
    {
#if defined( GL_COMPUTE_SHADER )
        case GL_COMPUTE_SHADER:
            return "GL_COMPUTE_SHADER";
#endif
        case GL_VERTEX_SHADER:
            return "GL_VERTEX_SHADER";
        case GL_TESS_CONTROL_SHADER:
            return "GL_TESS_CONTROL_SHADER";
        case GL_TESS_EVALUATION_SHADER:
            return "GL_TESS_EVALUATION_SHADER";
        case GL_GEOMETRY_SHADER:
            return "GL_GEOMETRY_SHADER";
        case GL_FRAGMENT_SHADER:
            return "GL_FRAGMENT_SHADER";
        default:
            break;
    }
    return "UNKNOWN_SHADER_TYPE";
}

} // namespace ltb::ogl
