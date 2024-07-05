// ///////////////////////////////////////////////////////////////////////////////////////
// A Logan Thomas Barnes project
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ltb/ogl/opengl.hpp"

// project
#include "ltb/ogl/fwd.hpp"
#include "ltb/ogl/type_traits.hpp"
#include "ltb/utils/result.hpp"

// standard
#include <functional>
#include <limits>
#include <memory>
#include <vector>

namespace ltb::ogl
{

auto set_defaults( ) -> void;

template < typename T = uint32_t >
constexpr auto default_restart_index( )
{
    return std::numeric_limits< T >::max( );
}

enum class WhenFinished
{
    RestorePreviousState,
    RestoreNullState,
    DoNothing,
};

/// \brief Convert a GLubyte string returned by glGetString to a std::string.
auto to_string( GLubyte const* str ) -> std::string;

/// \brief Return the result of glGetString as a std::string.
auto get_string( GLenum name ) -> std::string;

template < GLenum bind_type >
constexpr auto is_bindable_buffer_v = is_any_v<
    bind_type,
    GL_ARRAY_BUFFER,
    GL_ATOMIC_COUNTER_BUFFER,
    GL_COPY_READ_BUFFER,
    GL_COPY_WRITE_BUFFER,
    GL_DRAW_INDIRECT_BUFFER,
    GL_DISPATCH_INDIRECT_BUFFER,
    GL_ELEMENT_ARRAY_BUFFER,
    GL_PIXEL_PACK_BUFFER,
    GL_PIXEL_UNPACK_BUFFER,
    GL_SHADER_STORAGE_BUFFER,
    GL_TRANSFORM_FEEDBACK_BUFFER,
    GL_UNIFORM_BUFFER >;

template < GLenum bind_type >
constexpr auto is_bindable_texture_v = is_any_v<
    bind_type,
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_3D,
    GL_TEXTURE_1D_ARRAY,
    GL_TEXTURE_2D_ARRAY,
    GL_TEXTURE_RECTANGLE,
    GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_CUBE_MAP_ARRAY,
    GL_TEXTURE_BUFFER,
    GL_TEXTURE_2D_MULTISAMPLE,
    GL_TEXTURE_2D_MULTISAMPLE_ARRAY >;

template < GLenum bind_type >
constexpr auto is_bindable_framebuffer_v
    = is_any_v< bind_type, GL_FRAMEBUFFER, GL_DRAW_FRAMEBUFFER, GL_READ_FRAMEBUFFER >;

template < GLenum bind_type >
constexpr auto is_bindable_v
    = is_bindable_buffer_v< bind_type > || is_bindable_texture_v< bind_type >
   || is_bindable_framebuffer_v< bind_type >;

template < GLenum bind_type >
    requires is_bindable_v< bind_type >
constexpr auto binding_getter_type( ) -> GLenum
{
    switch ( bind_type )
    {
            // Buffers
        case GL_ARRAY_BUFFER:
            return GL_ARRAY_BUFFER_BINDING;
        case GL_ATOMIC_COUNTER_BUFFER:
            return GL_ATOMIC_COUNTER_BUFFER_BINDING;
        case GL_COPY_READ_BUFFER:
            return GL_COPY_READ_BUFFER_BINDING;
        case GL_COPY_WRITE_BUFFER:
            return GL_COPY_WRITE_BUFFER_BINDING;
        case GL_DRAW_INDIRECT_BUFFER:
            return GL_DRAW_INDIRECT_BUFFER_BINDING;
        case GL_DISPATCH_INDIRECT_BUFFER:
            return GL_DISPATCH_INDIRECT_BUFFER_BINDING;
        case GL_ELEMENT_ARRAY_BUFFER:
            return GL_ELEMENT_ARRAY_BUFFER_BINDING;
        case GL_PIXEL_PACK_BUFFER:
            return GL_PIXEL_PACK_BUFFER_BINDING;
        case GL_PIXEL_UNPACK_BUFFER:
            return GL_PIXEL_UNPACK_BUFFER_BINDING;
        case GL_SHADER_STORAGE_BUFFER:
            return GL_SHADER_STORAGE_BUFFER_BINDING;
        case GL_TRANSFORM_FEEDBACK_BUFFER:
            return GL_TRANSFORM_FEEDBACK_BUFFER_BINDING;
        case GL_UNIFORM_BUFFER:
            return GL_UNIFORM_BUFFER_BINDING;

            // Textures
        case GL_TEXTURE_1D:
            return GL_TEXTURE_BINDING_1D;
        case GL_TEXTURE_2D:
            return GL_TEXTURE_BINDING_2D;
        case GL_TEXTURE_3D:
            return GL_TEXTURE_BINDING_3D;
        case GL_TEXTURE_1D_ARRAY:
            return GL_TEXTURE_BINDING_1D_ARRAY;
        case GL_TEXTURE_2D_ARRAY:
            return GL_TEXTURE_BINDING_2D_ARRAY;
        case GL_TEXTURE_RECTANGLE:
            return GL_TEXTURE_BINDING_RECTANGLE;
        case GL_TEXTURE_CUBE_MAP:
            return GL_TEXTURE_BINDING_CUBE_MAP;
        case GL_TEXTURE_CUBE_MAP_ARRAY:
            return GL_TEXTURE_BINDING_CUBE_MAP_ARRAY;
        case GL_TEXTURE_BUFFER:
            return GL_TEXTURE_BINDING_BUFFER;
        case GL_TEXTURE_2D_MULTISAMPLE:
            return GL_TEXTURE_BINDING_2D_MULTISAMPLE;
        case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
            return GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY;

            // Framebuffer
        case GL_FRAMEBUFFER:
            return GL_FRAMEBUFFER_BINDING;
        case GL_DRAW_FRAMEBUFFER:
            return GL_DRAW_FRAMEBUFFER_BINDING;
        case GL_READ_FRAMEBUFFER:
            return GL_READ_FRAMEBUFFER_BINDING;

        default:
            break;
    }

    return GL_INVALID_ENUM;
}

} // namespace ltb::ogl
