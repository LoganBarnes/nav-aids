#pragma once

// graphics
#include "opengl.hpp"

// project
#include "ltb/ogl/fwd.hpp"
#include "ltb/utils/result.hpp"

// external
#include <spdlog/spdlog.h>

// standard
#include <memory>
#include <vector>

namespace ltb::ogl
{

template < typename DeleteFunc >
struct ArrayDeleter
{
    std::vector< GLuint > ids;
    DeleteFunc            delete_func;
    std::string           debug_type_str;

    template < typename Ignored >
    auto operator( )( Ignored const* const unused ) const
    {
        utils::ignore( unused );
        spdlog::debug( "{}({})", debug_type_str, fmt::join( ids, "," ) );
        delete_func( static_cast< GLsizei >( ids.size( ) ), ids.data( ) );
    }
};

template < typename DeleteFunc >
auto make_array_deleter(
    std::vector< GLuint > ids,
    DeleteFunc            delete_func,
    std::string           debug_type_str
) -> std::shared_ptr< void >
{
    constexpr void* null_object = nullptr;

    return std::shared_ptr< void >(
        // The pointer value is ignored. This shared_ptr is only used for the custom deleter.
        null_object,
        ArrayDeleter{ std::move( ids ), delete_func, std::move( debug_type_str ) }
    );
}

template < typename DeleteFunc >
struct Deleter
{
    GLuint      id = 0U;
    DeleteFunc  delete_func;
    std::string debug_type_str;

    template < typename Ignored >
    auto operator( )( Ignored const* const unused ) const
    {
        utils::ignore( unused );
        spdlog::debug( "{}({})", debug_type_str, id );
        delete_func( id );
    }
};

template < typename DeleteFunc >
auto make_deleter( GLuint id, DeleteFunc delete_func, std::string debug_type_str )
    -> std::shared_ptr< void >
{
    constexpr void* null_object = nullptr;

    return std::shared_ptr< void >(
        // The pointer value is ignored. This shared_ptr is only used for the custom deleter.
        null_object,
        Deleter{ id, delete_func, std::move( debug_type_str ) }
    );
}

} // namespace ltb::ogl
