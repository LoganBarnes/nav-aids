#pragma once

// graphics
#include "opengl.hpp"

// project
#include "ltb/math/range.hpp"
#include "ltb/ogl/bound.hpp"
#include "ltb/ogl/type_traits.hpp"

// standard
#include <memory>
#include <vector>

namespace ltb::ogl
{

/// \brief All the data the Texture class stores and manages.
struct TextureData
{
    /// \brief OpenGL ID from `glGenTextures()`
    GLuint gl_id = 0u;
};

/// \brief
class Texture
{
public:
    Texture( ) = default;

    /// \brief Initialize the texture object. This must
    ///        be called before using the texture.
    auto initialize( ) -> void;

    /// \brief The raw settings stored for this texture.
    [[nodiscard( "Const getter" )]]
    auto data( ) const -> TextureData const&;

    /// \brief Set the active texture unit.
    auto active_tex( GLint active_tex ) const -> void;

    static auto static_bind( GLenum type, Texture const& texture ) -> void;
    static auto static_bind( GLenum type, GLuint raw_gl_id ) -> void;

private:
    TextureData             data_    = { };
    std::shared_ptr< void > deleter_ = nullptr;
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

class TexParams
{
public:
    static auto filter( ) -> std::vector< GLenum >;
    static auto wrap( ) -> std::vector< GLenum >;
};

/// \brief Set a texture parameter.
template < GLenum BindingType >
    requires is_tex_parameteri_type< BindingType >
auto tex_parameteri(
    Bound< Texture, BindingType > bound_texture,
    std::vector< GLenum > const&  params,
    GLint const                   value
) -> void
{
    // This argument is provided to ensure the texture
    // is bound, but nothing has to be done with it.
    utils::ignore( bound_texture );

    for ( GLenum const param : params )
    {
        glTexParameteri( BindingType, param, value );
    }
}

/// \brief Copy the provided pixel data to this texture.
/// \param size The size of the texture.
/// \param pixels The input CPU pixel data.
/// \param internal_format The format used to store the data on the GPU.
/// \param format The format of the input pixel data.
/// \param type The type of the input pixel data.
/// \param level The level-of-detail number. 0 is the base level.
template < typename PixelData, GLenum BindingType >
    requires is_tex_image_2d_type< BindingType >
auto tex_image_2d(
    Bound< Texture, BindingType > const& bound_texture,
    glm::ivec2 const&                    size,
    PixelData const* const               pixels,
    GLint const                          internal_format,
    GLenum const                         format,
    GLenum const                         type,
    GLint const                          level
) -> void
{
    // This argument is provided to ensure the texture
    // is bound, but nothing has to be done with it.
    utils::ignore( bound_texture );

    // From the OpenGL documentation:
    // "This value must be 0."
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
    constexpr auto border = 0;

    glTexImage2D(
        BindingType,
        level,
        internal_format,
        size.x,
        size.y,
        border,
        format,
        type,
        pixels
    );
}

/// \brief Copy the provided pixel data to a sub rectangle of this texture.
/// \param sub_rect The bounds of the sub rectangle.
/// \param pixels The input CPU pixel data.
/// \param format The format of the input pixel data.
/// \param type The type of the input pixel data.
/// \param level The level-of-detail number. 0 is the base level.
template < typename PixelData, GLenum BindingType >
    requires is_tex_image_2d_type< BindingType >
auto tex_sub_image_2d(
    Bound< Texture, BindingType > bound_texture,
    math::Range2Di const&         sub_rect,
    PixelData const* const        pixels,
    GLenum const                  format,
    GLenum const                  type,
    GLint const                   level
) -> void
{
    // This argument is provided to ensure the texture
    // is bound, but nothing has to be done with it.
    utils::ignore( bound_texture );

    auto const sub_rect_size = dimensions( sub_rect );

    glTexSubImage2D(
        BindingType,
        level,
        sub_rect.min.x,
        sub_rect.min.y,
        sub_rect_size.x,
        sub_rect_size.y,
        format,
        type,
        pixels
    );
}

/// \brief Generate sub-sampled level of detail images for this texture.
template < GLenum BindingType >
    requires is_generate_mipmap_type< BindingType >
auto generate_mipmap( Bound< Texture, BindingType > bound_texture ) -> void
{
    // This argument is provided to ensure the texture
    // is bound, but nothing has to be done with it.
    utils::ignore( bound_texture );

    glGenerateMipmap( BindingType );
}

} // namespace ltb::ogl
