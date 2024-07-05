#pragma once

// project
#include "ltb/ogl/fwd.hpp"
#include "ltb/utils/result.hpp"

// standard
#include <string>

namespace ltb::ogl
{

// Specialise this function to support custom uniform types
template < typename ValueType >
struct UniformSetter
{
    auto operator( )( Program const& program, std::string const& name, ValueType const& value )
        const -> utils::Result< Program const* >;
};

} // namespace ltb::ogl
