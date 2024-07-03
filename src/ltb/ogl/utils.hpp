// ///////////////////////////////////////////////////////////////////////////////////////
// A Logan Thomas Barnes project
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ltb/ogl/opengl.hpp"

// project
#include "ltb/ogl/fwd.hpp"
#include "ltb/utils/generic_guard.hpp"

// standard
#include <functional>
#include <limits>
#include <memory>
#include <vector>

namespace ltb::ogl
{

auto set_defaults( ) -> void;

constexpr auto default_restart_index( )
{
    return ( std::numeric_limits< unsigned >::max )( );
}

enum class WhenFinished
{
    RestorePreviousState,
    RestoreNullState,
    DoNothing,
};

auto binding_type( GLenum type ) -> GLenum;

struct DestructionCallback
{
    explicit DestructionCallback( std::function< void( ) > callback );
    ~DestructionCallback( );

private:
    std::function< void( ) > callback_;
};

} // namespace ltb::ogl
