#include "ltb/ogl/vertex_array.hpp"

// project
#include "ltb/ogl/object_id.hpp"

namespace ltb::ogl
{

auto VertexArray::initialize( ) -> utils::Result<>
{
    glGenVertexArrays( 1, &data_.gl_id );

    spdlog::debug( "glGenVertexArrays({})", data_.gl_id );
    deleter_ = make_array_deleter( { data_.gl_id }, glDeleteVertexArrays, "glDeleteVertexArrays" );

    return utils::success( );
}

auto VertexArray::is_initialized( ) const -> bool
{
    return data( ).gl_id != 0U;
}

auto VertexArray::data( ) const -> VertexArrayData const&
{
    return data_;
}

auto VertexArray::static_bind( VertexArray const& vertex_array ) -> void
{
    VertexArray::static_bind( vertex_array.data( ).gl_id );
}

auto VertexArray::static_bind( GLuint const raw_gl_id ) -> void
{
    glBindVertexArray( raw_gl_id );
}

} // namespace ltb::ogl
