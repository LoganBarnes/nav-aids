#pragma once

// graphics
#include "opengl.hpp"

// project
#include "ltb/ogl/bound.hpp"
#include "ltb/ogl/type_traits.hpp"
#include "ltb/utils/result.hpp"

// standard
#include <memory>
#include <vector>

namespace ltb::ogl
{

/// \brief All the data the Texture class stores and manages.
struct TextureData
{
    GLuint                   gl_id      = 0u; ///< OpenGL ID from `glGenTextures()`.
    std::array< GLsizei, 3 > dimensions = { 0, 0, 0 };
    GLenum                   format     = GL_RGBA;
    GLenum                   type       = GL_FLOAT;
};

/// \brief
class Texture
{
public:
    explicit Texture( NoInitT );
    Texture( );

    /// \brief The raw settings stored for this texture
    [[nodiscard]] auto data( ) const -> TextureData const&;

    /// \brief Set the active texture unit.
    auto active_tex( GLint active_tex ) -> Texture&;

    static auto static_bind( GLenum type, Texture const& texture ) -> void;
    static auto static_bind( GLenum type, GLuint raw_gl_id ) -> void;

private:
    std::shared_ptr< TextureData > data_ = nullptr; ///< The RAII data.
};

// Specific types for bound texture functions
template < GLenum T >
constexpr auto is_tex_parameteri_type = is_any_v< T, GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP >;

template < GLenum T >
constexpr auto is_tex_image_2d_type = is_any_v<
    T,
    GL_TEXTURE_2D,
    GL_TEXTURE_1D_ARRAY,
    GL_TEXTURE_RECTANGLE,
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z >;

template < GLenum T >
constexpr auto is_generate_mipmap_type = is_any_v<
    T,
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_3D,
    GL_TEXTURE_1D_ARRAY,
    GL_TEXTURE_2D_ARRAY,
    GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_CUBE_MAP_ARRAY >;

struct TexParams
{
    static auto filter( ) -> std::vector< GLenum >;
    static auto wrap( ) -> std::vector< GLenum >;
};

/// \brief Set a texture parameter.
template < GLenum BindingType >
    requires is_tex_parameteri_type< BindingType >
auto tex_parameteri( Bound< Texture, BindingType > bound_texture, std::vector< GLenum > const& params, GLint value )
    -> utils::Result< Bound< Texture, BindingType > >
{
    for ( GLenum param : params )
    {
        glTexParameteri( BindingType, param, value );
    }
    return bound_texture;
}

/// \brief Set the texture filter and wrap parameters.
template < GLenum BindingType >
    requires is_tex_parameteri_type< BindingType >
auto filter_and_wrap_tex_parameteri( Bound< Texture, BindingType > bound_texture, GLint filter_type, GLint wrap_type )
    -> utils::Result< Bound< Texture, BindingType > >
{
    return tex_parameteri( bound_texture, TexParams::filter( ), filter_type )
        .and_then( tex_parameteri< BindingType >, TexParams::wrap( ), wrap_type );
}

/// \brief Copy the provided pixel data to this texture.
/// \param size The size of the texture.
/// \param pixels The input CPU pixel data.
/// \param internal_format The format used to store the data on the GPU.
/// \param format The format of the input pixel data.
/// \param type The type of the input pixel data.
/// \param level The level-of-detail number. 0 is the base level.
template < GLenum BindingType, typename PixelData >
    requires is_tex_image_2d_type< BindingType >
auto tex_image_2d(
    Bound< Texture, BindingType >   bound_texture,
    std::array< GLsizei, 2 > const& size,
    PixelData const* const          pixels          = nullptr,
    GLint const                     internal_format = GL_RGBA32F,
    GLenum const                    format          = GL_RGBA,
    GLenum const                    type            = GL_FLOAT,
    GLint const                     level           = 0
) -> utils::Result< Bound< Texture, BindingType > >
{
    // From the OpenGL docs: "This value must be 0."
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
    constexpr auto border = 0;

    bound_texture.object( ).data( ).dimensions = { size[ 0 ], size[ 1 ], 0 };
    bound_texture.object( ).data( ).format     = format;
    bound_texture.object( ).data( ).type       = type;
    glTexImage2D( BindingType, level, internal_format, size[ 0 ], size[ 1 ], border, format, type, pixels );
    return bound_texture;
}

/// \brief Copy the provided pixel data to a sub rectangle of this texture.
/// \param pos The position of the sub rectangle.
/// \param size The size of the sub rectangle.
/// \param pixels The input CPU pixel data.
/// \param format The format of the input pixel data.
/// \param type The type of the input pixel data.
/// \param level The level-of-detail number. 0 is the base level.
template < GLenum BindingType, typename PixelData >
    requires is_tex_image_2d_type< BindingType >
auto tex_sub_image_2d(
    Bound< Texture, BindingType >   bound_texture,
    std::array< GLint, 2 > const&   pos,
    std::array< GLsizei, 2 > const& size,
    PixelData const* const          pixels,
    GLenum const                    format = GL_RGBA,
    GLenum const                    type   = GL_FLOAT,
    GLint const                     level  = 0
) -> utils::Result< Bound< Texture, BindingType > >
{
    glTexSubImage2D( BindingType, level, pos[ 0 ], pos[ 1 ], size[ 0 ], size[ 1 ], format, type, pixels );
    return bound_texture;
}

/// \brief Generate sub-sampled level of detail images for this texture.
template < GLenum BindingType >
    requires is_generate_mipmap_type< BindingType >
auto generate_mipmap( Bound< Texture, BindingType > bound_texture ) -> utils::Result< Bound< Texture, BindingType > >
{
    glGenerateMipmap( BindingType );
    return bound_texture;
}

} // namespace ltb::ogl
