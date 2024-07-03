// ///////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2022 Rotor Technologies, Inc - All Rights Reserved
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "opengl.hpp"

// project
//#include "ltb/ogl/buffer.hpp"
#include "ltb/ogl/fwd.hpp"
#include "ltb/ogl/type_traits.hpp"
#include "ltb/ogl/utils.hpp"
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
    Texture( NoInitT ); // NOLINT(google-explicit-constructor)
    Texture( );

    /// \brief The raw settings stored for this texture
    [[nodiscard]] auto data( ) const -> TextureData const&;

    static auto static_bind( GLenum type, Texture const& texture ) -> void;
    static auto static_bind( GLenum type, GLuint raw_gl_id ) -> void;

    auto active_tex( GLint active_tex ) -> Texture&;

    template < GLenum E >
    class Bound;

    template < GLenum Type = GL_TEXTURE_2D >
    [[nodiscard]] auto bind( ) const -> Bound< Type >;

    template < GLenum Type = GL_TEXTURE_2D >
    [[nodiscard]] auto bind( WhenFinished when_finished = WhenFinished::RestoreNullState ) const -> Bound< Type >;

private:
    std::shared_ptr< TextureData > data_ = nullptr; ///< The RAII data.
};

template < GLenum Type >
auto Texture::bind( ) const -> Bound< Type >
{
    return Bound< Type >{ *this, WhenFinished::RestoreNullState };
}

template < GLenum Type >
auto Texture::bind( WhenFinished when_finished ) const -> Bound< Type >
{
    return Bound< Type >{ *this, when_finished };
}

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
constexpr auto is_tex_image_2d_multisample_type = is_any_v< T, GL_TEXTURE_2D_MULTISAMPLE >;
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

/// \brief
/// \tparam Type
template < GLenum Type >
class Texture::Bound
{
public:
    explicit Bound( Texture texture, WhenFinished when_finished );

    /// \brief Set a texture parameter.
    template < GLenum T = Type >
        requires is_tex_parameteri_type< T >
    auto tex_parameteri( std::vector< GLenum > const& params, GLint value ) -> Bound&;

    /// \brief Set the texture filter and wrap parameters.
    template < GLenum T = Type >
        requires is_tex_parameteri_type< T >
    auto filter_and_wrap_tex_parameteri( GLint filter_type = GL_LINEAR, GLint wrap_type = GL_CLAMP_TO_EDGE ) -> Bound&;

    /// \brief Copy the provided pixel data to this texture.
    /// \param size The size of the texture.
    /// \param pixels The input CPU pixel data.
    /// \param internal_format The format used to store the data on the GPU.
    /// \param format The format of the input pixel data.
    /// \param type The type of the input pixel data.
    /// \param level The level-of-detail number. 0 is the base level.
    template < GLenum T = Type >
        requires is_tex_image_2d_type< T >
    auto tex_image_2d(
        std::array< GLsizei, 2 > const& size,
        void const*                     pixels          = nullptr,
        GLint                           internal_format = GL_RGBA32F,
        GLenum                          format          = GL_RGBA,
        GLenum                          type            = GL_FLOAT,
        GLint                           level           = 0
    ) -> Bound&;

    //    /// \brief Map the bound PBO to this texture.
    //    /// \param size The size of the texture.
    //    /// \param pbo The input PBO pixel data.
    //    /// \param internal_format The format used to store the data on the GPU.
    //    /// \param format The format of the input pixel data.
    //    /// \param type The type of the input pixel data.
    //    /// \param level The level-of-detail number. 0 is the base level.
    //    template < GLenum T = Type >
    //        requires is_tex_image_2d_type< T >
    //    auto tex_image_2d(
    //        std::array< GLsizei, 2 > const&                size,
    //        Buffer::Bound< GL_PIXEL_UNPACK_BUFFER > const& pbo,
    //        GLint                                          internal_format = GL_RGBA32F,
    //        GLenum                                         format          = GL_RGBA,
    //        GLenum                                         type            = GL_FLOAT,
    //        GLint                                          level           = 0
    //    ) -> Bound&;

    /// \brief Copy the provided pixel data to a sub rectangle of this texture.
    /// \param pos The position of the sub rectangle.
    /// \param size The size of the sub rectangle.
    /// \param pixels The input CPU pixel data.
    /// \param format The format of the input pixel data.
    /// \param type The type of the input pixel data.
    /// \param level The level-of-detail number. 0 is the base level.
    template < GLenum T = Type >
        requires is_tex_image_2d_type< T >
    auto tex_sub_image_2d(
        std::array< GLint, 2 > const&   pos,
        std::array< GLsizei, 2 > const& size,
        void const*                     pixels,
        GLenum                          format = GL_RGBA,
        GLenum                          type   = GL_FLOAT,
        GLint                           level  = 0
    ) -> Bound&;

    //    /// \brief Map the bound PBO to a sub rectangle of this texture.
    //    /// \param pos The position of the sub rectangle.
    //    /// \param size The size of the sub rectangle.
    //    /// \param pixels The input PBO pixel data.
    //    /// \param format The format of the input pixel data.
    //    /// \param type The type of the input pixel data.
    //    /// \param level The level-of-detail number. 0 is the base level.
    //    template < GLenum T = Type >
    //        requires is_tex_image_2d_type< T >
    //    auto tex_sub_image_2d(
    //        std::array< GLint, 2 > const&                  pos,
    //        std::array< GLsizei, 2 > const&                size,
    //        Buffer::Bound< GL_PIXEL_UNPACK_BUFFER > const& pbo,
    //        GLenum                                         format = GL_RGBA,
    //        GLenum                                         type   = GL_FLOAT,
    //        GLint                                          level  = 0
    //    ) -> Bound&;

    /// \brief Copy the provided pixel data to this multisample texture.
    template < GLenum T = Type >
        requires is_tex_image_2d_multisample_type< T >
    auto tex_image_2d_multisample(
        std::array< GLsizei, 2 > const& size,
        GLsizei                         samples,
        GLenum                          internal_format        = GL_RGBA32F,
        GLboolean                       fixed_sample_locations = GL_FALSE
    ) -> Bound&;

    /// \brief Generate subsampled level of detail images for this texture.
    template < GLenum T = Type >
        requires is_generate_mipmap_type< T >
    auto generate_mipmap( ) -> Bound&;

    /// \brief A convenience function to end a chain of function calls without
    ///        the compiler complaining about unused return values.
    auto done( ) const -> void {
        // Nothing should be implemented here.
    };

    /// \brief The original texture.
    [[nodiscard]] auto texture( ) const -> Texture const&;

    /// \brief The type used when binding this texture (GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, etc).
    [[nodiscard]] auto type( ) const -> GLenum;

private:
    Texture                                texture_;
    std::shared_ptr< DestructionCallback > on_destruction_ = nullptr;
};

template < GLenum Type >
template < GLenum T >
    requires is_tex_parameteri_type< T >
auto Texture::Bound< Type >::tex_parameteri( std::vector< GLenum > const& params, GLint value ) -> Bound< Type >&
{
    for ( GLenum param : params )
    {
        glTexParameteri( Type, param, value );
    }
    return *this;
}

template < GLenum Type >
template < GLenum T >
    requires is_tex_parameteri_type< T >
auto Texture::Bound< Type >::filter_and_wrap_tex_parameteri( GLint filter_type, GLint wrap_type ) -> Bound< Type >&
{
    return tex_parameteri( TexParams::filter( ), filter_type ).tex_parameteri( TexParams::wrap( ), wrap_type );
}

template < GLenum Type >
template < GLenum T >
auto Texture::Bound< Type >::tex_image_2d(
    std::array< GLsizei, 2 > const& size,
    void const*                     pixels,
    GLint                           internal_format,
    GLenum                          format,
    GLenum                          type,
    GLint                           level
) -> Bound< Type >&
{
    // From the OpenGL docs: "This value must be 0."
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
    constexpr auto border = 0;

    texture_.data_->dimensions = { size[ 0 ], size[ 1 ], 0 };
    texture_.data_->format     = format;
    texture_.data_->type       = type;
    glTexImage2D( Type, level, internal_format, size[ 0 ], size[ 1 ], border, format, type, pixels );
    return *this;
}

template < GLenum Type >
template < GLenum T >
auto Texture::Bound< Type >::tex_image_2d(
    std::array< GLsizei, 2 > const& size,
    Buffer::Bound< GL_PIXEL_UNPACK_BUFFER > const& /*pbo*/,
    GLint  internal_format,
    GLenum format,
    GLenum type,
    GLint  level
) -> Bound< Type >&
{
    return tex_image_2d( size, nullptr, internal_format, format, type, level );
}

template < GLenum Type >
template < GLenum T >
auto Texture::Bound< Type >::tex_sub_image_2d(
    std::array< GLint, 2 > const&   pos,
    std::array< GLsizei, 2 > const& size,
    void const*                     pixels,
    GLenum                          format,
    GLenum                          type,
    GLint                           level
) -> Bound< Type >&
{
    glTexSubImage2D( Type, level, pos[ 0 ], pos[ 1 ], size[ 0 ], size[ 1 ], format, type, pixels );
    return *this;
}

template < GLenum Type >
template < GLenum T >
auto Texture::Bound< Type >::tex_sub_image_2d(
    std::array< GLint, 2 > const&   pos,
    std::array< GLsizei, 2 > const& size,
    Buffer::Bound< GL_PIXEL_UNPACK_BUFFER > const& /*pbo*/,
    GLenum format,
    GLenum type,
    GLint  level
) -> Bound< Type >&
{
    return tex_sub_image_2d( pos, size, nullptr, format, type, level );
}

template < GLenum Type >
template < GLenum T >
auto Texture::Bound< Type >::tex_image_2d_multisample(
    std::array< GLsizei, 2 > const& size,
    GLsizei                         samples,
    GLenum                          internal_format,
    GLboolean                       fixed_sample_locations
) -> Bound< Type >&
{
    texture_.data_->dimensions = { size[ 0 ], size[ 1 ], 0 };
    glTexImage2DMultisample( Type, samples, internal_format, size[ 0 ], size[ 1 ], fixed_sample_locations );
    return *this;
}

template < GLenum Type >
template < GLenum T >
auto Texture::Bound< Type >::generate_mipmap( ) -> Bound< Type >&
{
    glGenerateMipmap( Type );
    return *this;
}

} // namespace ltb::ogl
