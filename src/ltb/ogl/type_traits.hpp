// ///////////////////////////////////////////////////////////////////////////////////////
// A Logan Thomas Barnes project
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

// opengl
#include "opengl.hpp"

// standard
#include <type_traits>

namespace ltb::ogl
{

template < GLenum T, GLenum... Ts >
struct is_any : std::bool_constant< ( ( T == Ts ) || ... ) >
{
};

template < GLenum T, GLenum... Ts >
constexpr auto is_any_v = is_any< T, Ts... >::value;

} // namespace ltb::ogl
