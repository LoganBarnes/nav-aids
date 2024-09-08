#pragma once

#include "ltb/ogl/opengl.hpp"

// project
#include "ltb/ogl/bound.hpp"
#include "ltb/ogl/fwd.hpp"
#include "ltb/ogl/type_traits.hpp"

// standard
#include <memory>
#include <vector>

namespace ltb::ogl
{

/// \brief All the data the Buffer class stores and manages.
struct BufferData
{
    /// \brief OpenGL ID from `glGenBuffers()`
    GLuint gl_id = 0U;
};

/// \brief A class to manage OpenGL buffers.
class Buffer
{
public:
    Buffer( ) = default;

    /// \brief Initialize the buffer object. This must
    ///        be called before using the buffer.
    auto initialize( ) -> utils::Result<>;

    /// \brief Check if the buffer has been successfully initialized.
    [[nodiscard( "Const getter" )]]
    auto is_initialized( ) const -> bool;

    /// \brief The raw settings stored for this buffer
    [[nodiscard( "Const getter" )]]
    auto data( ) const -> BufferData const&;

    static auto static_bind( GLenum type, Buffer const& buffer ) -> void;
    static auto static_bind( GLenum type, GLuint raw_gl_id ) -> void;

private:
    BufferData              data_    = { };
    std::shared_ptr< void > deleter_ = nullptr;
};

template < GLenum bind_type >
concept IsBufferType = IsAnyOf<
    bind_type,
    GL_ARRAY_BUFFER,
    GL_ATOMIC_COUNTER_BUFFER,
    GL_COPY_READ_BUFFER,
    GL_COPY_WRITE_BUFFER,
    GL_DISPATCH_INDIRECT_BUFFER,
    GL_DRAW_INDIRECT_BUFFER,
    GL_ELEMENT_ARRAY_BUFFER,
    GL_PIXEL_PACK_BUFFER,
    GL_PIXEL_UNPACK_BUFFER,
    GL_QUERY_BUFFER,
    GL_SHADER_STORAGE_BUFFER,
    GL_TEXTURE_BUFFER,
    GL_TRANSFORM_FEEDBACK_BUFFER,
    GL_UNIFORM_BUFFER >;

template < GLenum bind_type >
concept IsBindBufferRangeType = IsAnyOf<
    bind_type,
    GL_ATOMIC_COUNTER_BUFFER,
    GL_TRANSFORM_FEEDBACK_BUFFER,
    GL_UNIFORM_BUFFER,
    GL_SHADER_STORAGE_BUFFER >;

/// \brief Reallocate the buffer data on the GPU
/// \param data is the data to store in the buffer (this can be null to fill with zeros)
/// \param num_elements is the number of elements the buffer should store
/// \param usage is the OpenGL intended usage hint
template < typename DataType, GLenum bind_type >
    requires IsBufferType< bind_type >
auto buffer_data(
    Bound< Buffer, bind_type > const& bound_buffer,
    std::size_t const                 num_elements,
    DataType const* const             data,
    GLenum const                      usage
) -> void
{
    // This argument is provided to ensure the buffer
    // is bound, but nothing has to be done with it.
    utils::ignore( bound_buffer );

    auto const size_in_bytes = static_cast< GLsizeiptr >( num_elements * sizeof( DataType ) );
    glBufferData( bind_type, size_in_bytes, data, usage );
}

/// \brief Reallocate the buffer data on the GPU
/// \param data the data to store in the buffer
/// \param usage is the OpenGL intended usage hint
template < typename DataType, GLenum bind_type >
    requires IsBufferType< bind_type >
auto buffer_data(
    Bound< Buffer, bind_type > const& bound_buffer,
    std::vector< DataType > const&    data,
    GLenum                            usage
) -> void
{
    return buffer_data( bound_buffer, data.size( ), data.data( ), usage );
}

/// \brief Update a section of the buffer.
/// \param data is the data to copy to the buffer.
/// \param num_elements is the number of elements to copy.
/// \param start_index is the offset into the buffer where the copy should start.
template < typename DataType, GLenum bind_type >
    requires IsBufferType< bind_type >
auto buffer_sub_data(
    Bound< Buffer, bind_type > const& bound_buffer,
    DataType const* const             data,
    std::size_t const                 num_elements,
    GLintptr const                    offset_bytes
) -> void
{
    // This argument is provided to ensure the buffer
    // is bound, but nothing has to be done with it.
    utils::ignore( bound_buffer );

    auto const size_in_bytes = static_cast< GLsizeiptr >( num_elements * sizeof( DataType ) );
    glBufferSubData( bind_type, offset_bytes, size_in_bytes, data );
}

/// \brief Update a section of the buffer.
/// \param data is the data to copy to the buffer.
/// \param start_index is the offset into the buffer where the copy should start.
template < typename DataType, GLenum bind_type >
    requires IsBufferType< bind_type >
auto buffer_sub_data(
    Bound< Buffer, bind_type > const& bound_buffer,
    std::vector< DataType > const&    data,
    GLintptr const                    offset_bytes
) -> void
{
    return buffer_sub_data( bound_buffer, data.data( ), data.size( ), offset_bytes );
}

/// \brief Retrieve data from the buffer.
/// \param data is the location where the retrieved data will be stored.
/// \param num_elements is the number of elements to copy.
/// \param start_index is the offset into the buffer where the copy should start.
template < typename DataType, GLenum bind_type >
    requires IsBufferType< bind_type >
auto get_buffer_sub_data(
    Bound< Buffer, bind_type > const& bound_buffer,
    DataType* const                   data,
    GLsizeiptr const                  num_elements,
    GLintptr const                    start_index
) -> void
{
    // This argument is provided to ensure the buffer
    // is bound, but nothing has to be done with it.
    utils::ignore( bound_buffer );

    auto const start_byte    = static_cast< GLintptr >( start_index * sizeof( DataType ) );
    auto const size_in_bytes = static_cast< GLsizeiptr >( num_elements * sizeof( DataType ) );
    glGetBufferSubData( bind_type, start_byte, size_in_bytes, data );
}

template < GLenum bind_type >
    requires IsBindBufferRangeType< bind_type >
auto bind_buffer_range(
    Bound< Buffer, bind_type > const& bound_buffer,
    GLuint const                      index,
    GLintptr const                    byte_offset,
    GLsizeiptr const                  size_in_bytes
) -> void
{
    auto const& buffer = bound_buffer.object( );

    glBindBufferRange( bind_type, index, buffer.data( ).gl_id, byte_offset, size_in_bytes );
}

} // namespace ltb::ogl
