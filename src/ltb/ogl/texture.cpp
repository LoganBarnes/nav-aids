#include "ltb/ogl/texture.hpp"

// project
#include "ltb/ogl/raw_opengl_utils.hpp"

namespace ltb::ogl
{

Texture::Texture( NoInitT ) {}

Texture::Texture( )
    : data_( make_shared_data< TextureData >( glGenTextures, glDeleteTextures ) )
{
}

auto Texture::data( ) const -> TextureData const&
{
    return *data_;
}

auto Texture::active_tex( GLint const active_tex ) -> Texture&
{
    glActiveTexture( static_cast< GLenum >( GL_TEXTURE0 + active_tex ) );
    return *this;
}

auto Texture::static_bind( GLenum type, Texture const& texture ) -> void
{
    Texture::static_bind( type, texture.data( ).gl_id );
}

auto Texture::static_bind( GLenum type, GLuint raw_gl_id ) -> void
{
    glBindTexture( type, raw_gl_id );
}

constexpr auto is_texture( GLenum type ) -> bool
{
    switch ( type )
    {
        case GL_TEXTURE_1D:
        case GL_TEXTURE_2D:
        case GL_TEXTURE_3D:
        case GL_TEXTURE_1D_ARRAY:
        case GL_TEXTURE_2D_ARRAY:
        case GL_TEXTURE_RECTANGLE:
        case GL_TEXTURE_CUBE_MAP:
        case GL_TEXTURE_CUBE_MAP_ARRAY:
        case GL_TEXTURE_BUFFER:
        case GL_TEXTURE_2D_MULTISAMPLE:
        case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
            return true;
        default:
            break;
    }
    return false;
}

auto TexParams::filter( ) -> std::vector< GLenum >
{
    return {
        GL_TEXTURE_MIN_FILTER,
        GL_TEXTURE_MAG_FILTER,
    };
}

auto TexParams::wrap( ) -> std::vector< GLenum >
{
    return {
        GL_TEXTURE_WRAP_S,
        GL_TEXTURE_WRAP_T,
    };
}

//template class Bound< Texture, GL_TEXTURE_1D >;
//template class Bound< Texture, GL_TEXTURE_2D >;
//template class Bound< Texture, GL_TEXTURE_3D >;
//template class Bound< Texture, GL_TEXTURE_1D_ARRAY >;
//template class Bound< Texture, GL_TEXTURE_2D_ARRAY >;
//template class Bound< Texture, GL_TEXTURE_RECTANGLE >;
//template class Bound< Texture, GL_TEXTURE_CUBE_MAP >;
//template class Bound< Texture, GL_TEXTURE_CUBE_MAP_ARRAY >;
//template class Bound< Texture, GL_TEXTURE_BUFFER >;
//template class Bound< Texture, GL_TEXTURE_2D_MULTISAMPLE >;
//template class Bound< Texture, GL_TEXTURE_2D_MULTISAMPLE_ARRAY >;

} // namespace ltb::ogl
