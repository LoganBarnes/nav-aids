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
class IsAny : public std::bool_constant< ( ( T == Ts ) || ... ) >
{
};

template < GLenum T, GLenum... Ts >
constexpr auto is_any_v = IsAny< T, Ts... >::value;

template < typename T, typename... Ts >
class IsAnySame : public std::bool_constant< ( ( std::is_same_v< T, Ts > ) || ... ) >
{
};

template < typename T, typename... Ts >
constexpr auto is_any_same_v = IsAnySame< T, Ts... >::value;

} // namespace ltb::ogl
