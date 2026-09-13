#pragma once

// project
#include "ltb/ogl/buffer.hpp"
#include "ltb/ogl/program.hpp"
#include "ltb/ogl/program_attribute.hpp"
#include "ltb/ogl/program_uniform.hpp"
#include "ltb/ogl/shader.hpp"
#include "ltb/ogl/type_traits.hpp"
#include "ltb/ogl/vertex_array.hpp"

// external
#include <pfr.hpp>
// #include <spdlog/spdlog.h>

// standard
#include <tuple>
#include <utility>

namespace ltb::utils
{

template < typename T >
concept Initializable = requires( T a ) {
    { a.initialize( ) } -> std::same_as< Result<> >;
    { a.is_initialized( ) } -> std::same_as< bool >;
};

struct FailedResult
{
    Result<> result;

    template < typename Type >
        requires Initializable< Type >
    auto operator( )( Type& object ) -> bool
    {
        if ( object.is_initialized( ) )
        {
            result = success( );
        }
        else
        {
            result = object.initialize( );
        }
        return !result;
    }
};

template < typename... Objects >
auto initialize( Objects&... objects ) -> Result<>
{
    if ( auto failed_result = FailedResult{ }; ( failed_result( objects ) || ... ) )
    {
        return failed_result.result;
    }
    return success( );
}

#if 0

template < typename... Objects, std::size_t... Is >
auto initialize( std::tuple< Objects&... > const& objects, std::index_sequence< Is... > )
    -> Result<>
{
    return initialize( std::get< Is >( objects )... );
}

template < typename... Objects >
auto initialize( std::tuple< Objects&... > const& objects ) -> Result<>
{
    return initialize( objects, std::index_sequence_for< Objects... >( ) );
}

/// \warning This function can increase compilation time a bit.
template < typename Object >
auto initialize( Object& object ) -> Result<>
{
    return initialize( pfr::structure_tie( object ) );
}

#endif

} // namespace ltb::utils
