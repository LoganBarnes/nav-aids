#include "ltb/ogl/buffer.hpp"

// project
#include "ltb/ogl/object_id.hpp"

namespace ltb::ogl
{

auto Buffer::initialize( ) -> void
{
    glGenBuffers( 1, &data_.gl_id );

    spdlog::debug( "glGenBuffers({})", data_.gl_id );
    deleter_ = make_array_deleter( { data_.gl_id }, glDeleteBuffers, "glDeleteBuffers" );
}

auto Buffer::is_initialized( ) const -> bool
{
    return 0U != data_.gl_id;
}

auto Buffer::data( ) const -> BufferData const&
{
    return data_;
}

auto Buffer::static_bind( GLenum const type, Buffer const& buffer ) -> void
{
    Buffer::static_bind( type, buffer.data( ).gl_id );
}

auto Buffer::static_bind( GLenum const type, GLuint const raw_gl_id ) -> void
{
    glBindBuffer( type, raw_gl_id );
}

} // namespace ltb::ogl
