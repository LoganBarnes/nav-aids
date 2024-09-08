#include "ltb/ogl/texture.hpp"

// project
#include "ltb/ogl/object_id.hpp"

namespace ltb::ogl
{

auto Texture::initialize( ) -> void
{
    glGenTextures( 1, &data_.gl_id );

    spdlog::debug( "glGenTextures({})", data_.gl_id );
    deleter_ = make_array_deleter( { data_.gl_id }, glDeleteTextures, "glDeleteTextures" );
}

auto Texture::is_initialized( ) const -> bool
{
    return 0U != data_.gl_id;
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
