#include "ltb/ogl/texture.hpp"

namespace ltb::ogl
{

struct Deleter
{
    GLuint id;

    template < typename Ignored >
    auto operator( )( Ignored const* const ) const
    {
        glDeleteTextures( 1, &id );
    }
};

auto Texture::initialize( ) -> utils::Result<>
{
    glGenTextures( 1, &data_.gl_id );
    if ( 0u == data_.gl_id )
    {
        return LTB_MAKE_UNEXPECTED_ERROR( "Failed to generate texture ID" );
    }

    deleter_ = std::shared_ptr< void >( this, Deleter{ data_.gl_id } );

    return utils::success( );
}

auto Texture::data( ) const -> TextureData const&
{
    return data_;
}

auto Texture::active_tex( GLint const active_tex ) const -> void
{
    glActiveTexture( static_cast< GLenum >( GL_TEXTURE0 + active_tex ) );
}

auto Texture::static_bind( GLenum const type, Texture const& texture ) -> void
{
    Texture::static_bind( type, texture.data( ).gl_id );
}

auto Texture::static_bind( GLenum const type, GLuint const raw_gl_id ) -> void
{
    glBindTexture( type, raw_gl_id );
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

} // namespace ltb::ogl
