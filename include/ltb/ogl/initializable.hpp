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
#include <spdlog/spdlog.h>

// standard
#include <filesystem>
#include <tuple>
#include <utility>

namespace ltb::ogl
{

template < typename T >
concept Initializable = requires( T a ) {
    { a.initialize( ) } -> std::same_as< utils::Result<> >;
    { a.is_initialized( ) } -> std::same_as< bool >;
};

struct FailedResult
{
    utils::Result<> result;

    template < typename Type >
        requires Initializable< Type >
    auto operator( )( Type& object ) -> bool
    {
        if ( object.is_initialized( ) )
        {
            result = utils::success( );
        }
        else
        {
            result = object.initialize( );
        }
        return !result;
    }
};

template < typename... Objects >
auto initialize( Objects&... objects ) -> utils::Result<>
{
    if ( auto failed_result = FailedResult{ }; ( failed_result( objects ) || ... ) )
    {
        return failed_result.result;
    }
    return utils::success( );
}

template < typename... Objects, std::size_t... Is >
auto initialize( std::tuple< Objects&... > const& objects, std::index_sequence< Is... > )
    -> utils::Result<>
{
    return initialize( std::get< Is >( objects )... );
}

template < typename... Objects >
auto initialize( std::tuple< Objects&... > const& objects ) -> utils::Result<>
{
    return initialize( objects, std::index_sequence_for< Objects... >( ) );
}

template < typename Object >
auto initialize( Object& object ) -> utils::Result<>
{
    return initialize( pfr::structure_tie( object ) );
}

} // namespace ltb::ogl
